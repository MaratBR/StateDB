#pragma once
#include "pch.h"
#include "connection.h"

_BEGIN_STATEDB_NET

class asio_server
{
public:
	using io_pointer = std::shared_ptr<boost::asio::io_context>;
	using acceptor_pointer = std::unique_ptr<boost::asio::ip::tcp::acceptor>;

	asio_server(boost::asio::ip::tcp::endpoint ep)
	{
		io_service_ = io_pointer(new boost::asio::io_context);
		acceptor_ = acceptor_pointer(new boost::asio::ip::tcp::acceptor(*io_service_, ep));
		address_string = boost::str(boost::format("%s:%d") % ep.address().to_string() % ep.port());
		logger_ = logging::get_server_logger(address_string);
		logger_->debug("New ASIO server instantiated: {}:{}", ep.address().to_string(), ep.port());
	}

	void start_listening() 
	{
		if (is_listening_) return;
		is_listening_ = true;

		start_accept();
	}

	void run()
	{
		io_service_->run();
	}
	bool is_listening() const
	{
		return is_listening_;
	}
private:
	void start_accept()
	{
		if (!is_listening_)
			return;

		logger_->debug("Waiting for a connection...");
		tcp_connection::pointer connection =
			tcp_connection::create(io_service_);

		acceptor_->async_accept(connection->socket(),
			boost::bind(&asio_server::handle_accept, this, connection,
				boost::asio::placeholders::error));
	}
	void handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec);

	io_pointer io_service_;
	acceptor_pointer acceptor_;
	bool is_listening_ = false;
	std::shared_ptr<spdlog::logger> logger_;
	std::string address_string;
};

_END_STATEDB_NET