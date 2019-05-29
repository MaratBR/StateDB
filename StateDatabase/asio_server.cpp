#include "pch.h"
#include "asio_server.h"

_BEGIN_STATEDB_NET

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
		connection->start();
	}

	start_accept();
}

_END_STATEDB_NET