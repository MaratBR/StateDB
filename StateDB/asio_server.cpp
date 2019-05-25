#include "stdafx.h"
#include "asio_server.h"
#include <boost/bind.hpp>

using namespace boost;

statedb::net::asio_server::asio_server(tcp::endpoint ep)
	: acceptor_(io_service_, ep)
{
	spdlog::debug("New ASIO server insantiated: {}:{}", ep.address().to_string(), ep.port());
}

void statedb::net::asio_server::listen()
{
	if (is_listening_) return;
	is_listening_ = true;

	while (is_listening_)
	{
		start_accept();
	}
}

bool statedb::net::asio_server::is_listening() const
{
	return is_listening_;
}

void statedb::net::asio_server::start_accept()
{
	if (!is_listening_)
		return;

	spdlog::debug("Waiting for a connection...");
	tcp_connection::pointer connection =
		tcp_connection::create(io_service_);

	acceptor_.async_accept(connection->socket(), 
		boost::bind(&asio_server::handle_accept, this, connection,
			boost::asio::placeholders::error));
}

void statedb::net::asio_server::handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec)
{
	if (ec)
	{
		spdlog::error("Failed to accept connection. Error: {}", ec.message());
	}
	else
	{
		connection->start();
	}

	start_accept();
}
