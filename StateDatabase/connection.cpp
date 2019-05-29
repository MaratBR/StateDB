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
	auto buf = dynamic_storage_.allocate_asio_buffer<hello_message_static>();

	return;

	_STATEDB_UTILS read_with_timeout(
		socket_,
		buf,
		HELLO_TIMEOUT,
		boost::bind(&tcp_connection::received_hello, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred),
		boost::bind(&tcp_connection::timer_expired, this, boost::asio::placeholders::error),
		boost::ref(*deadline_timer_)
	);
}

void tcp_connection::close()
{
	
}

void tcp_connection::init()
{
	deadline_timer_ = std::unique_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(*io_context_, boost::posix_time::seconds(4)));
	address_string = boost::str(
		boost::format("%s:%d") % socket_.remote_endpoint().address().to_string()
		% socket_.remote_endpoint().port());
	logger = logging::get_connection_logger(address_string);
}

void tcp_connection::received_hello(const BOOST_ERR_CODE& ec, size_t br)
{
	logger->info("Received HELLO from {},", address_string);
}

void tcp_connection::timer_expired(const BOOST_ERR_CODE& ec)
{
	logger->info("Timeout for {} expired with error code: {}, action: {}", address_string, ec.message(), timed_out_action);
}

void tcp_connection::timer_expired_fatal(const BOOST_ERR_CODE& ec)
{
	timer_expired(ec);
	close();
}

tcp_connection::tcp_connection(std::shared_ptr<boost::asio::io_context> io_context_)
	: socket_(*io_context_),
	io_context_(io_context_)
{
}

void tcp_connection::_async_read_message_or_close__read(boost::function<void(const BOOST_ERR_CODE&, size_t)> h, const BOOST_ERR_CODE& ec, size_t bt)
{
	if (ec.failed())
	{
		logger->warn("{} - Reading operation failed: {}", address_string, ec.message());
		timeout_close();
	}
	else
	{
		h(boost::ref(ec), bt);
	}
}

void tcp_connection::_async_read_message_or_close__timeout(const BOOST_ERR_CODE& ec)
{
	if (ec.failed())
	{
		logger->warn("{} - Timeout failed with error {}", ec.message());
	}
	timeout_close();
}

_END_STATEDB_NET