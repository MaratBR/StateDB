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
	init_dispatcher();
}

void asio_server::start_listening()
{
	if (is_listening_) return;
	is_listening_ = true;

	start_stats_task();
	start_accept();
}

void asio_server::handle_message(tcp_connection& conn, message_preamble& preamble)
{
	logger_->debug("Got message (ID: {}) from {}", preamble.id, conn.get_id());

	bool hasHandler = dispatcher_.call(preamble.id, conn);
	if (!hasHandler)
	{
		logger_->debug("Command {} not recognized", preamble.id);
	}
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
		connection->on_close(boost::bind(&asio_server::handle_connection_close, this, boost::placeholders::_1));
		connection->on_new_message(boost::bind(&asio_server::handle_message, this, boost::placeholders::_1, boost::placeholders::_2));
		connection->start();
	}

	start_accept();
}

void asio_server::handle_connection_close(tcp_connection& conn)
{
	io_service_.post(boost::bind(&asio_server::handle_connection_remove, this, conn.get_id()));
}

void asio_server::handle_connection_remove(std::string connId)
{
	connections.erase(connId);
	logger_->debug("Connection {} were removed from hashmap", connId);
}

void asio_server::init_dispatcher()
{
	logger_->debug("Initializing dispatcher...");
	//dispatcher_.register_handler<>();
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