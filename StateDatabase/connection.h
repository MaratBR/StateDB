#pragma once
#include "pch.h"
#include "asio_stream.h"
#include "dynamic_storage.h"
#include "logging.h"
#include "net_utils.h"


_BEGIN_STATEDB_NET

class tcp_connection
{
public:
	struct pending_read_operation_base
	{
		virtual void perform(tcp_connection& ds) = 0;
		boost::function<void(const BOOST_ERR_CODE&, size_t)> handler;
	};
	
	template<typename T>
	struct pending_read_operation
	{
		virtual void perform(tcp_connection& ds) override
		{
			ds.async_read_message<T>(
				
				);
		}
	};

	using pointer = std::shared_ptr<tcp_connection>;

	static tcp_connection::pointer create(boost::asio::io_context& io_context_)
	{
		return pointer(new tcp_connection(io_context_));
	}

	ASIO_SOCKET& socket();

	void start();

	void close();

	void init();

	std::string get_id() const;
private:
	void received_hello(const BOOST_ERR_CODE& ec, size_t br);

	inline void timeout_close()
	{
		spdlog::info("{} - Connection closed due to timeout expiration", address_string);
		close();
	}

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

	tcp_connection(boost::asio::io_context& io_context_);

	void handle_timeout_expiration(const BOOST_ERR_CODE& ec);

	void handle_data(
		const BOOST_ERR_CODE& ec, 
		size_t bt, 
		bool required, 
		boost::function<void(const BOOST_ERR_CODE&, size_t)> next
	);

	void start_read_message();

	void handle_message(const BOOST_ERR_CODE& ec, size_t bt);

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
		logger->debug("{} bytes reserved for {}, dynamic storage allocated", sizeof(T), typename_);
		auto buf = dynamic_storage_.allocate_asio_buffer<T>();

		// Read the data
		socket_.async_read_some(
			buf, 
			boost::bind(
				&tcp_connection::handle_data, 
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				required,
				h
				)
		);
		if (timeout.ticks() != 0)
		{
			// Run timeout if specified
			deadline_timer_.expires_from_now(timeout);
			deadline_timer_.async_wait(
				boost::bind(
					&tcp_connection::handle_timeout_expiration, 
					this, 
					boost::asio::placeholders::error
				)
			);
		}
	}

	// Common case for async_read_message function. Reads data with deafault timeout.
	// Operation is assummed to be required.
	template<typename T>
	inline void async_read_message(boost::function<void(const BOOST_ERR_CODE&, size_t)> h)
	{
		async_read_message<T>(h, TIMEOUT, true);
	}

	boost::posix_time::milliseconds TIMEOUT = STATEDB_NET_TIMEOUT;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::io_context& io_context_;
	boost::asio::deadline_timer deadline_timer_;
	std::string timed_out_action = ""; // DEPRECATED?
	std::string address_string;
	std::shared_ptr<spdlog::logger> logger = nullptr;

	_STATEDB_UTILS dynamic_storage dynamic_storage_;
};

_END_STATEDB_NET