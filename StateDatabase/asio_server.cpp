#include "pch.h"
#include "asio_server.h"
#include "format_utils.h"

_BEGIN_STATEDB_NET

asio_server::asio_server(boost::asio::ip::tcp::endpoint ep)
	: acceptor_(io_service_, ep),
	stats_timer(io_service_)
{
	address_string = boost::str(boost::format("%s:%d") % ep.address().to_string() % ep.port());
	logger_ = logging::get_server_logger(address_string);
	logger_->debug("New ASIO server instantiated: {}:{}", ep.address().to_string(), ep.port());
}

void asio_server::start_listening()
{
	if (is_listening_) return;
	is_listening_ = true;

	start_stats_task();
	start_accept();
}

void asio_server::handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec)
{
	connection->init();
	if (!is_listening_)
	{
		if (!ec)
		{
			connection->socket().close();
		}
	}

	if (ec)
	{
		logger_->error("Failed to accept connection. Error: {}", ec.message());
	}
	else
	{
		logger_->debug("New TCP connection established: {}:{}", connection->socket().remote_endpoint().address().to_string(), connection->socket().remote_endpoint().port());
		connections[connection->get_id()] = connection;
		connection->start();
	}

	start_accept();
}

void asio_server::start_stats_task()
{
	stats_timer.expires_from_now(STATEDB_SERVER_STATS_TASK_TIMEOUT);
	stats_timer.async_wait(boost::bind(&asio_server::stats_printer, this, boost::asio::placeholders::error));
}

void asio_server::stats_printer(const BOOST_ERR_CODE& ec)
{
	if (!is_listening_)
		return;
	if (ec.failed())
	{
		logger_->warn("Stats timer error: {}, {}", ec.value(), ec.message());
	}
	logger_->info(
		"Uptime: {}, requests served: {}, active connections: ({})",
		_STATEDB_UTILS format_duration(std::chrono::system_clock::now() - start_time),
		"undefined",
		connections.size()
	);

	start_stats_task();
}

_END_STATEDB_NET