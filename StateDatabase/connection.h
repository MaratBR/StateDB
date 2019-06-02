#pragma once
#include "pch.h"
#include "asio_stream.h"
#include "dynamic_storage.h"
#include "logging.h"
#include "net_utils.h"
#include "net_message.h"
#include "empty_handler.h"

/* Main connection class. 
 * Implements read and write operations, timeout and pending operation queue.
 * TODOS:
 *	- Unify read/write operation methods for dynamic (aka "raw binary") and static operations (the ones that can be used for reading particular structure from socket) 
 */


_BEGIN_STATEDB_NET

class tcp_connection
{
public:
	static const size_t MAX_PENDING_COUNT = STATEDB_MAX_OPERATIONS_PENDING;

	struct pending_operation_base
	{
		pending_operation_base(
			boost::function<void(const BOOST_ERR_CODE&, size_t)> handler_,
			bool required_,
			boost::posix_time::milliseconds timeout_
		)
			: handler(handler_), timeout(timeout_), required(required_)
		{
		}

		virtual void perform(tcp_connection& ds) = 0;

		boost::function<void(const BOOST_ERR_CODE&, size_t)> handler;
		boost::posix_time::milliseconds timeout;
		bool required;
	};
	
	template<typename T>
	struct pending_read_operation : public pending_operation_base
	{
		using pending_operation_base::pending_operation_base;

		virtual void perform(tcp_connection& ds) override
		{
			ds.async_read_message<T>(handler);
		}
	};

	template<typename T>
	struct pending_write_operation : public pending_operation_base
	{
		using pending_operation_base::pending_operation_base;

		virtual void perform(tcp_connection& ds) override
		{
			//ds.async_read_message<T>(handler);
		}
	};

	struct pending_write_raw_operation : public pending_operation_base
	{
		pending_write_raw_operation(
			void* memory, 
			size_t size,
			boost::function<void(const BOOST_ERR_CODE&, size_t)> handler_,
			bool required_,
			boost::posix_time::milliseconds timeout_
		);

		virtual void perform(tcp_connection& ds) override;
		size_t size;
		void* memory;
	};

	struct pending_read_raw_operation : public pending_operation_base
	{
		pending_read_raw_operation(
			size_t s,
			boost::function<void(const BOOST_ERR_CODE&, size_t)> handler_,
			bool required_,
			boost::posix_time::milliseconds timeout_
		);

		virtual void perform(tcp_connection& ds) override;
		size_t size;
	};

	using pointer = std::shared_ptr<tcp_connection>;

	static tcp_connection::pointer create(boost::asio::io_context& io_context_)
	{
		return pointer(new tcp_connection(io_context_));
	}

	ASIO_SOCKET& socket();

	void start();

	void close();
	void close(BOOST_ERR_CODE ec);

	void init();
	_STATEDB_UTILS dynamic_storage& get_read_buffer();
	boost::posix_time::milliseconds get_timeout() const;

	std::string get_id() const;

	// Connects handler to `on-new-message` signal
	void on_new_message(
		boost::signals2::signal<void(tcp_connection&, message_preamble&)>::slot_type slot
	);

	// Connects handler to `on-before-closing` signal
	void on_close(
		boost::signals2::signal<void(tcp_connection&)>::slot_type slot
	);

	// Checks if error occur and returns true true and closes socket if so
	inline bool assert_no_error(const BOOST_ERR_CODE& ec)
	{
		if (ec.failed())
		{
			logger->error("Error occur: {}, {}", ec.value(), ec.message());
			close();
			return true;
		}
		return false;
	}

	inline void timeout_close()
	{
		logger->info("Connection closed due to timeout expiration");
		close();
	}

	// Asynchronously writes data to the socket
	template<typename T>
	void async_write_message(
		T& val,
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		boost::posix_time::milliseconds timeout,
		bool required = true
	)
	{
		static const char* typename_ = TYPE_NAME(T);

		if (write_in_progress)
		{
			logger->debug("Write operation for {} delayed due to another write operation being processed", typename_);
			pending_write_operations.push(
				std::shared_ptr<pending_operation_base>(new pending_read_operation<T>(h, required, timeout))
			);
			return;
		}
		write_in_progress = true;
		std::shared_ptr<bool> operation_done(new bool(false));

		logger->debug("{} bytes reserved for write operation for {}", sizeof(T), typename_);
		dynamic_write_storage_.copy_from(val);

		socket_.async_write_some(
			dynamic_write_storage_.get_asio_buffer(),
			boost::bind(
				&tcp_connection::handle_write_operation,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				required,
				h,
				operation_done
			)
		);

		if (timeout.ticks() != 0)
		{
			// Run timeout if specified
			run_timer(deadline_write_timer_, timeout, operation_done);
		}
	}

	void async_write_raw(
		void* memory,
		size_t amount,
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		boost::posix_time::milliseconds timeout,
		bool required = true
	);

	// Shortcut for @code async_write_message<T>(val, STATEDB_ASIO_EMPTY_HANDLER, timeout, required); @endcode
	template<typename T>
	inline void async_write_message(
		T& val,
		boost::posix_time::milliseconds timeout,
		bool required = true
	)
	{
		async_write_message<T>(val, STATEDB_ASIO_EMPTY_HANDLER, timeout, required);
	}

	// Shortcut for @code async_write_message<T>(val, h, TIMEOUT, required); @endcode
	// Where TIMEOUT - is default timeout for connection, defined as constant private member.
	template<typename T>
	inline void async_write_message(
		T& val,
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		bool required = true
	)
	{
		async_write_message<T>(val, h, TIMEOUT, required);
	}

	// Shortcut for 
	// @code async_read_message<T>(val, TIMEOUT); @endcode
	// OR
	// @code async_write_message<T>(val, STATEDB_ASIO_EMPTY_HANDLER, TIMEOUT, true); @endcode
	template<typename T>
	inline void async_write_message(
		T& val
	)
	{
		async_read_message<T>(val, TIMEOUT);
	}

	// Writes message of the given type to the socket.
	// Constructs object using default constructor.
	// This function is equivalent to:
	// @code
	//	T val;
	//	connectionObject.async_write_message(val);
	//	// val is out of scope now
	// @endcode
	template<typename T>
	inline void async_write_message()
	{
		T val;
		async_read_message<T>(val);
	}

	void async_read_raw(
		size_t amount,
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		boost::posix_time::milliseconds timeout,
		bool required = true
	);

	// Asynchronously reads data of the given type from socket
	// Closes connection if:
	// * `required` is true (default) and data cannot be read due to an error
	// * timer expired 
	template<typename T>
	void async_read_message(
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		boost::posix_time::milliseconds timeout,
		bool required = true
	)
	{
		static const char* typename_ = TYPE_NAME(T);
		if (read_in_progress)
		{
			logger->debug("Read operation for {} delayed due to another read operation being processed", typename_);
			pending_read_operations.push(
				std::shared_ptr<pending_operation_base>(new pending_read_operation<T>(h, required, timeout))
			);
			return;
		}
		read_in_progress = true;
		logger->debug("{} bytes reserved for {}, dynamic storage allocated", sizeof(T), typename_);
		auto buf = dynamic_storage_.allocate_asio_buffer<T>();
		std::shared_ptr<bool> operation_done(new bool(false));

		// Read the data
		socket_.async_read_some(
			buf,
			boost::bind(
				&tcp_connection::handle_data,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				required,
				h,
				operation_done
			)
		);
		if (timeout.ticks() != 0)
		{
			run_timer(deadline_timer_, timeout, operation_done);
		}
	}

	// Common case for async_read_message function. Reads data with default timeout.
	// Operation is assummed to be required.
	template<typename T>
	inline void async_read_message(boost::function<void(const BOOST_ERR_CODE&, size_t)> h)
	{
		async_read_message<T>(h, TIMEOUT);
	}

	// Allows pending operations to be perfomed by themselves
	template<typename T>
	friend struct pending_read_operation;

	template<typename T>
	friend struct pending_write_operation;
private:
	using pending_operations_queue = std::queue<std::shared_ptr<pending_operation_base>>;

	void received_hello(const BOOST_ERR_CODE& ec, size_t br);

	tcp_connection(boost::asio::io_context& io_context_);

	inline void run_timer(
		boost::asio::deadline_timer& timer,
		boost::posix_time::milliseconds timeout,
		std::shared_ptr<bool> operation_done
	)
	{
		// Run timeout if specified
		timer.expires_from_now(timeout);
		timer.async_wait(
			boost::bind(
				&tcp_connection::handle_timeout_expiration,
				this,
				boost::asio::placeholders::error,
				operation_done
			)
		);
	}

	void handle_timeout_expiration(const BOOST_ERR_CODE& ec, std::shared_ptr<bool> operationIsDone);

	void handle_data(
		const BOOST_ERR_CODE& ec, 
		size_t bt, 
		bool required, 
		boost::function<void(const BOOST_ERR_CODE&, size_t)> next,
		std::shared_ptr<bool> operationIsDone
	);

	void start_read_message();

	void handle_message(const BOOST_ERR_CODE& ec, size_t bt);

	void handle_write_operation(
		const BOOST_ERR_CODE& ec,
		size_t bt,
		bool required,
		boost::function<void(const BOOST_ERR_CODE&, size_t)> next,
		std::shared_ptr<bool> operationIsDone
	);

	boost::posix_time::milliseconds TIMEOUT = STATEDB_NET_TIMEOUT;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::io_context& io_context_;
	boost::asio::deadline_timer deadline_timer_;
	boost::asio::deadline_timer deadline_write_timer_;
	std::string timed_out_action = "";
	std::string address_string;
	std::shared_ptr<spdlog::logger> logger = nullptr;

	boost::signals2::signal<void(tcp_connection&, message_preamble&)> on_new_message_;
	boost::signals2::signal<void(tcp_connection&)> on_closed_;

	pending_operations_queue pending_read_operations;
	pending_operations_queue pending_write_operations;
	bool read_in_progress = false, write_in_progress = false;

	_STATEDB_UTILS dynamic_storage dynamic_storage_;
	_STATEDB_UTILS dynamic_storage dynamic_write_storage_;
};

_END_STATEDB_NET