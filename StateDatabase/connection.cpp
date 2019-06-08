#include "pch.h"
#include "connection.h"
#include "format_utils.h"

_BEGIN_STATEDB_NET

tcp_connection::pending_write_raw_operation::pending_write_raw_operation(
	const void* memory,
	size_t size,
	boost::function<void(const BOOST_ERR_CODE&, size_t)> handler_,
	bool required_,
	boost::posix_time::milliseconds timeout_
)
	: tcp_connection::pending_operation_base(handler_, required_, timeout_), size(size), memory(memory)
{
}

void tcp_connection::pending_write_raw_operation::perform(tcp_connection& conn)
{
	conn.async_write_raw(memory, size, handler, timeout, required);
}

tcp_connection::pending_read_raw_operation::pending_read_raw_operation(
	size_t s,
	boost::function<void(const BOOST_ERR_CODE&, size_t)> handler_,
	bool required_,
	boost::posix_time::milliseconds timeout_
)
	: tcp_connection::pending_operation_base(handler_, required_, timeout_), size(s)
{
}

void tcp_connection::pending_read_raw_operation::perform(tcp_connection& conn)
{
	conn.async_read_raw(size, handler, timeout, required);
}

ASIO_SOCKET& tcp_connection::socket()
{
	return socket_;
}

void tcp_connection::start()
{
	logger->debug("Waiting for HELLO from {}:{} ...", socket_.remote_endpoint().address().to_string(), socket_.remote_endpoint().port());
	async_read_message<hello_message_static>(
		boost::bind(&tcp_connection::received_hello, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred),
		TIMEOUT
		);
}

void tcp_connection::close()
{
	close(boost::system::errc::make_error_code(boost::system::errc::success));
}

void tcp_connection::close(BOOST_ERR_CODE ec)
{
	socket_.close();

	logger->info(
		L"Connection closed with error: {}, {}",
		ec.value(),
		_STATEDB_UTILS to_wstring(ec.message())
	);

	on_closed_(*this);
}

void tcp_connection::init()
{
	address_string = boost::str(
		boost::format("%s:%d") % socket_.remote_endpoint().address().to_string()
		% socket_.remote_endpoint().port());
	logger = logging::get_connection_logger(address_string);
}

_STATEDB_UTILS dynamic_storage& tcp_connection::get_read_buffer()
{
	return dynamic_storage_;
}

boost::posix_time::milliseconds tcp_connection::get_timeout() const
{
	return TIMEOUT;
}


// Returns ID (address string "IP:PORT" i.e. 123.7.45.9:12345) of the connection
std::string tcp_connection::get_id() const
{
	return address_string;
}

void tcp_connection::on_new_message(boost::signals2::signal<void(tcp_connection&, message_preamble&)>::slot_type slot)
{
	on_new_message_.connect(slot);
}

void tcp_connection::on_close(boost::signals2::signal<void(tcp_connection&)>::slot_type slot)
{
	on_closed_.connect(slot);
}

void tcp_connection::async_write_raw(const void* memory, size_t amount, boost::function<void(const BOOST_ERR_CODE&, size_t)> h, boost::posix_time::milliseconds timeout, bool required)
{
	if (amount == 0) return;
	if (write_in_progress)
	{
		logger->debug("Write operation for binary({}) delayed due to another write operation being processed", amount);
		pending_write_operations.push(
			std::shared_ptr<pending_operation_base>(new pending_write_raw_operation(memory, amount, h, required, timeout))
		);
		return;
	}
	write_in_progress = true;
	std::shared_ptr<bool> operation_done(new bool(false));

	logger->debug("{0} bytes reserved for write operation for binary", amount);
	dynamic_write_storage_.allocate_binary(amount, memory);

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

void tcp_connection::async_read_raw(size_t amount, boost::function<void(const BOOST_ERR_CODE&, size_t)> h, boost::posix_time::milliseconds timeout, bool required)
{
	if (read_in_progress)
	{
		logger->debug("Read operation for binary({}) delayed due to another read operation being processed", amount);
		pending_read_operations.push(
			std::shared_ptr<pending_operation_base>(new pending_read_raw_operation(amount, h, required, timeout))
		);
		return;
	}
	read_in_progress = true;
	logger->debug("{} bytes reserved for binary", amount);
	dynamic_storage_.allocate_binary(amount);
	std::shared_ptr<bool> operation_done(new bool(false));

	// Read the data
	socket_.async_read_some(
		dynamic_storage_.get_asio_buffer(),
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

// Handler for async operation - HELLO message
void tcp_connection::received_hello(const BOOST_ERR_CODE& ec, size_t br)
{
	hello_message_static& hello = dynamic_storage_.get<hello_message_static>();
	if (!hello.valid(STATEDB_PROTOCOL_VERSION))
	{
		logger->warn("Invalid HELLO message: '{}' '{}' '{}' '{}' '{}', protocol byte: {}",
			hello._HELLO[0], hello._HELLO[1], hello._HELLO[2], hello._HELLO[3], hello._HELLO[4], hello._HELLO[5],
			(uint16_t)(hello.protocol_version));
		close();
		dynamic_storage_.deallocate();
		return;
	}
	logger->info("Received HELLO from {}", address_string);
	dynamic_storage_.deallocate();

	start_read_message();
}

// Handles timeout expiration, closes connection when timeout expired
void tcp_connection::handle_timeout_expiration(const BOOST_ERR_CODE& ec, std::shared_ptr<bool> operationIsDone)
{
	if (*operationIsDone)
		return;
	logger->info(L"Timeout expired with error code: {} ({})",
		ec.value(),
		_STATEDB_UTILS to_wstring(ec.message())
	);
	close(ec);
}

// Handler for async operation - reading data from socket
void tcp_connection::handle_data(
	const BOOST_ERR_CODE& ec,
	size_t bt,
	bool required,
	boost::function<void(const BOOST_ERR_CODE&, size_t)> next,
	std::shared_ptr<bool> operationIsDone
)
{
	*operationIsDone = true;
	logger->debug(
		L"Received {} bytes, current dynamic storage: {}, success: {}, errno: {}, errmsg: {}",
		bt,
		_STATEDB_UTILS to_wstring(dynamic_storage_.get_type()),
		!ec.failed(),
		ec.value(),
		_STATEDB_UTILS to_wstring(ec.message())
	);
	// If operation failed but were required
	if (required && ec.failed())
	{
		logger->error("Read operation failed: {} {}", ec.value(), ec.message());
		close();
		return;
	}

	// Cancels timer
	auto successEC = boost::system::errc::make_error_code(boost::system::errc::success);
	deadline_timer_.cancel(successEC);
	read_in_progress = false;
	// Call next handler
	next(ec, bt);

	if (!pending_read_operations.empty())
	{
		auto op = pending_read_operations.front();
		pending_read_operations.pop();
		op->perform(*this);
	}
}

// Starts async. reading operation 
void tcp_connection::start_read_message()
{
	if (!socket_.is_open())
		return;

	async_read_message<message_preamble>(
		boost::bind(&tcp_connection::handle_message, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred),
		boost::posix_time::milliseconds(0),
		true
		);
}

// Handles new message's preamble
void tcp_connection::handle_message(const BOOST_ERR_CODE& ec, size_t bt)
{
	if (assert_no_error(ec)) return;

	message_preamble& msg = dynamic_storage_.get<message_preamble>();
	// Working with message
	logger->debug("Received message ID: {}", msg.id);

	// Call signal
	dynamic_storage_.deallocate(); // IMPORTANT Deallocate BEFORE calling signal
	on_new_message_(*this, msg);

	start_read_message();
}

void tcp_connection::handle_write_operation(
	const BOOST_ERR_CODE& ec,
	size_t bt,
	bool required,
	boost::function<void(const BOOST_ERR_CODE&, size_t)> next,
	std::shared_ptr<bool> operationIsDone
)
{
	*operationIsDone = true;
	logger->debug(
		L"{} bytes are sent, current dynamic write storage: {}, success: {}, errno: {}, errmsg: {}",
		bt,
		_STATEDB_UTILS to_wstring(dynamic_write_storage_.get_type()),
		!ec.failed(),
		ec.value(),
		_STATEDB_UTILS to_wstring(ec.message())
	);

	// If operation failed but were required
	if (required && ec.failed())
	{
		logger->error(L"Write operation failed: {}, {}", ec.value(), _STATEDB_UTILS to_wstring(ec.message()));
		close();
		return;
	}

	write_in_progress = false;
	// Cancel timer
	deadline_write_timer_.cancel();
	// Call next handler
	next(ec, bt);

	if (!pending_write_operations.empty())
	{
		auto op = pending_write_operations.front();
		pending_write_operations.pop();
		op->perform(*this);
	}
}

tcp_connection::tcp_connection(boost::asio::io_context& io_context_)
	: socket_(io_context_),
	io_context_(io_context_),
	deadline_timer_(io_context_),
	deadline_write_timer_(io_context_)
{
}

_END_STATEDB_NET


