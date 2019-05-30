#pragma once
#include "pch.h"
#include "connection.h"

_BEGIN_STATEDB_NET

class asio_server
{
public:
	using io_pointer = std::shared_ptr<boost::asio::io_context>;
	using acceptor_pointer = std::unique_ptr<boost::asio::ip::tcp::acceptor>;

	asio_server(boost::asio::ip::tcp::endpoint ep);

	void start_listening();

	void run()
	{
		io_service_.run();
	}
	bool is_listening() const
	{
		return is_listening_;
	}
private:
	void start_stats_task();

	void start_accept()
	{
		if (!is_listening_)
			return;

		logger_->debug("Waiting for a connection...");
		tcp_connection::pointer connection =
			tcp_connection::create(io_service_);

		acceptor_.async_accept(connection->socket(),
			boost::bind(&asio_server::handle_accept, this, connection,
				boost::asio::placeholders::error));
	}

	void stats_printer(const BOOST_ERR_CODE& ec);

	void handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec);

	boost::asio::io_context io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	bool is_listening_ = false;
	std::shared_ptr<spdlog::logger> logger_;
	std::string address_string;
	std::map<std::string, tcp_connection::pointer> connections;
	boost::asio::deadline_timer stats_timer;
	std::chrono::system_clock::time_point start_time;
};

_END_STATEDB_NET