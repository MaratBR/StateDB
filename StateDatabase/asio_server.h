#pragma once
#include "pch.h"
#include "connection.h"
#include "dispatcher.h"
#include "db.h"

_BEGIN_STATEDB_NET

class asio_server
{
public:
	using io_pointer = std::shared_ptr<boost::asio::io_context>;
	using acceptor_pointer = std::unique_ptr<boost::asio::ip::tcp::acceptor>;

	struct handlers
	{

		// Handler that loads message body before calling inner handler
		struct process_message
		{
			process_message(
				boost::function<void(tcp_connection&, asio_server&, processed_message&)>
			);

			static void handle_proccessed_data(
				const BOOST_ERR_CODE& ec,
				size_t bt, 
				process_message& pmsg,
				tcp_connection& conn, 
				asio_server& server,
				message_preamble msgp
			);

			void operator()(tcp_connection&, asio_server&, message_preamble&);

			boost::function<void(tcp_connection&, asio_server&, processed_message&)> _handler;
		};

		// Responds with PONG message
		struct ping_handler
		{
			void operator()(tcp_connection&, asio_server&, message_preamble&);
		};

		struct get_handler
		{
			void operator()(tcp_connection&, asio_server&, processed_message&);

			static void send_key_not_found(tcp_connection&, const char* key);
			static void send_value(tcp_connection& conn, asio_server& server, db_object& obj);
			static void send_value(tcp_connection& conn, asio_server& server, const char* key);
		};

		struct set_handler
		{
			void operator()(tcp_connection&, asio_server&, processed_message&);
		};

		struct delete_handler
		{
			void operator()(tcp_connection&, asio_server&, processed_message&);
		};

		struct get_all_handler
		{
			void operator()(tcp_connection&, asio_server&, message_preamble&);
		};

		struct clear_all
		{
			void operator()(tcp_connection&, asio_server&, message_preamble&);
		};

		struct inc_handler
		{
			void operator()(tcp_connection&, asio_server&, message_preamble&);
		};

		struct dec_handler
		{
			void operator()(tcp_connection&, asio_server&, message_preamble&);
		};
	};

	asio_server(boost::asio::ip::tcp::endpoint ep, db& db_);

	void start_listening();

	void run()
	{
		io_service_.run();
	}
	bool is_listening() const
	{
		return is_listening_;
	}
	void apply_all_connections(
		boost::function<void(tcp_connection&)> h
	);
private:
	void init_dispatcher();

	void start_stats_task();

	void start_accept()
	{
		if (!is_listening_)
			return;

		logger_->info("Waiting for a connection...");
		tcp_connection::pointer connection =
			tcp_connection::create(io_service_);

		acceptor_.async_accept(connection->socket(),
			boost::bind(&asio_server::handle_accept, this, connection,
				boost::asio::placeholders::error));
	}

	void stats_printer(const BOOST_ERR_CODE& ec);

	void handle_message(tcp_connection& conn, message_preamble& preamble);
	void handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec);
	void handle_connection_close(tcp_connection& conn);
	void handle_connection_remove(std::string connId);

	db& db_;
	boost::asio::io_context io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	bool is_listening_ = false;
	std::shared_ptr<spdlog::logger> logger_;
	std::string address_string;
	std::map<std::string, tcp_connection::pointer> connections;
	boost::asio::deadline_timer stats_timer;
	std::chrono::system_clock::time_point start_time;
	_STATEDB_UTILS dispatcher<commands::command_t, tcp_connection&, asio_server&, message_preamble&> dispatcher_;
};

_END_STATEDB_NET