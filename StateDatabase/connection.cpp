#include "pch.h"
#include "connection.h"
#include "net_message.h"

_BEGIN_STATEDB_NET

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
	logger->info("Connection closed");
	socket_.close();
}

void tcp_connection::init()
{
	address_string = boost::str(
		boost::format("%s:%d") % socket_.remote_endpoint().address().to_string()
		% socket_.remote_endpoint().port());
	logger = logging::get_connection_logger(address_string);
}

std::string tcp_connection::get_id() const
{
	return address_string;
}

void tcp_connection::received_hello(const BOOST_ERR_CODE& ec, size_t br)
{
	hello_message_static& hello = dynamic_storage_.get<hello_message_static>();
	if (!hello.valid(STATEDB_PROTOCOL_VERSION))
	{
		logger->warn("Invalid HELLO message: '{}' '{}' '{}' '{}' '{}', protocol byte: {}", 
			hello._HELLO[0], hello._HELLO[1], hello._HELLO[2], hello._HELLO[3], hello._HELLO[4], hello._HELLO[5],
			static_cast<int>(hello.protocol_version));
		close();
		dynamic_storage_.deallocate();
		return;
	}
	logger->info("Received HELLO from {}", address_string);
	dynamic_storage_.deallocate();

	
}

void tcp_connection::handle_timeout_expiration(const BOOST_ERR_CODE& ec)
{
	logger->info("Timeout expired with error code: {} ({}), action: {}", address_string, ec.value(), ec.message(), timed_out_action);
	close();
}

void tcp_connection::handle_data(const BOOST_ERR_CODE& ec, size_t bt, bool required, boost::function<void(const BOOST_ERR_CODE&, size_t)> next)
{
	logger->debug(
		"Received {} bytes, current dynamic storage: {}, success: {}, errno: {}, errmsg: {}", 
		bt, 
		dynamic_storage_.get_type(),
		!ec.failed(),
		ec.value(),
		ec.message()
		);
	// If operation failed but required
	if (required && ec.failed())
	{
		logger->error("Read oparation failed: {} {}", ec.value(), ec.message());
		close();
		return;
	}

	// Cancels timer
	deadline_timer_.cancel();
	// Call next handler
	next(ec, bt);
}

void tcp_connection::start_read_message()
{
	async_read_message<message_preamble>(
		boost::bind(&tcp_connection::handle_message, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
		);
}

void tcp_connection::handle_message(const BOOST_ERR_CODE& ec, size_t bt)
{
	if (assert_no_error(ec)) return;

	message_preamble& msg = dynamic_storage_.get<message_preamble>();
	// Working with message

	if (!msg.valid())
	{

		dynamic_storage_.deallocate();
		return;
	}

	// Deallocate message
	dynamic_storage_.deallocate();
}

tcp_connection::tcp_connection(boost::asio::io_context& io_context_)
	: socket_(io_context_),
	io_context_(io_context_),
	deadline_timer_(io_context_)
{
}

_END_STATEDB_NET