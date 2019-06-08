#include "pch.h"
#include "asio_server.h"
#include "format_utils.h"
#include "hash.h"

_BEGIN_STATEDB_NET

asio_server::asio_server(boost::asio::ip::tcp::endpoint ep, db& db_)
	: acceptor_(io_service_, ep),
	stats_timer(io_service_),
	db_(db_)
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

	bool hasHandler = dispatcher_.call(preamble.id, conn, *this, preamble);
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

void asio_server::apply_all_connections(boost::function<void(tcp_connection&)> h)
{
	for (auto& p : connections)
		h(*p.second);
}

void asio_server::init_dispatcher()
{
	logger_->debug("Initializing dispatcher...");
	dispatcher_.register_handler<handlers::ping_handler>(commands::request_ping);
	dispatcher_.register_handler(commands::request_get, handlers::process_message(handlers::get_handler()));
	dispatcher_.register_handler(commands::request_set, handlers::process_message(handlers::set_handler()));
	dispatcher_.register_handler<handlers::get_all_handler>(commands::request_get_all);
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


/// MESSAGE HANDLER

asio_server::handlers::process_message::process_message(boost::function<void(tcp_connection&, asio_server&, processed_message&)> h)
	: _handler(h)
{
}

void asio_server::handlers::process_message::handle_proccessed_data(
	const BOOST_ERR_CODE&, 
	size_t, 
	process_message& pmsg, 
	tcp_connection& conn, 
	asio_server& server,
	message_preamble msgp
)
{
	processed_message msg(msgp, conn.get_read_buffer().get_binary());
	pmsg._handler(conn, server, msg);
}

void asio_server::handlers::process_message::operator()(tcp_connection& conn, asio_server& server, message_preamble& msgp)
{
	// Load message body
	conn.async_read_raw(
		msgp.size,
		boost::bind(
			&process_message::handle_proccessed_data, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred,
			boost::ref(*this),
			boost::ref(conn),
			boost::ref(server),
			msgp
		),
		conn.get_timeout()
		);
}

void asio_server::handlers::ping_handler::operator()(tcp_connection& conn, asio_server&, message_preamble&)
{
	conn.async_write_message<make_pong_message>();
}

void asio_server::handlers::get_handler::operator()(tcp_connection& conn, asio_server& server, processed_message& msgp)
{
	char* key = msgp.as_cstr();
	server.logger_->debug("Received GET for key '{}'", key);
	send_value(conn, server, key);
}

void asio_server::handlers::get_handler::send_key_not_found(tcp_connection& conn, const char* key)
{
	// Send key-not-found message
	make_key_deleted_preamble preamble;
	preamble.size = strlen(key) + 1;

	// Data inside processed_message will be gone soon, so we should copy that
	char* deletedKey = new char[preamble.size];
	std::memcpy(deletedKey, key, preamble.size);

	conn.async_write_message(preamble);
	conn.async_write_raw(deletedKey, preamble.size, [deletedKey](const BOOST_ERR_CODE&, size_t) { delete[] deletedKey; }, conn.get_timeout());
}

void asio_server::handlers::get_handler::send_value(tcp_connection& conn, asio_server& server, db_object& obj)
{
	dtypes::dtype_t type = obj.get_type();
	std::string key = obj.get_key();

	constexpr size_t typeIdSize = sizeof(dtypes::dtype_t);
	size_t 
		objSize = obj.get_size(),
		keySize = key.length() + 1;

	size_t size = typeIdSize + objSize + keySize;

	make_key_updated_preamble preamble;
	preamble.size = size;

	char* data = new char[size];

	// Copy zero-terminated key
	std::memcpy(data, key.data(), key.length());
	data[key.length()] = '\0';

	char* objMemory = data + keySize;
	obj.write_to(objMemory + typeIdSize, objSize);
	*reinterpret_cast<dtypes::dtype_t*>(objMemory ) = obj.get_type();

	std::shared_ptr<char> dataPtr(data, [](char* ptr) { delete[] ptr; });
	conn.async_write_message(
		preamble,
		[dataPtr, data, &conn, size](const BOOST_ERR_CODE&, size_t)
		{

			conn.async_write_raw(
				data,
				size,
				[dataPtr](const BOOST_ERR_CODE&, size_t) {},
				conn.get_timeout()
			);
		}
	);
}

void asio_server::handlers::get_handler::send_value(tcp_connection& conn, asio_server& server, const char* key)
{
	size_t keyHash = make_hash(key);
	boost::optional<db_object*> obj = server.db_.get_object(keyHash);

	if (obj.has_value())
	{
		// Send value to user
		db_object& objRef = *obj.value();
		send_value(conn, server, objRef);
	}
	else
	{
		send_key_not_found(conn, key);
	}
}

void asio_server::handlers::set_handler::operator()(tcp_connection& conn, asio_server& server, processed_message& pmsg)
{
	size_t keySize = 0;
	char* chBuf = reinterpret_cast<char*>(pmsg.buffer);
	while (keySize < pmsg.size && chBuf[keySize] != '\0')
	{
		keySize++;
	}

	if (pmsg.size - keySize < 1 + sizeof(dtypes::dtype_t))
	{
		// Not enought space for actual data
		conn.async_write_error("You have sent not enough data for DTYPE");
		return;
	}

	chBuf[keySize] = '\0';
	
	dtypes::dtype_t dtype = *reinterpret_cast<dtypes::dtype_t*>(chBuf + keySize + 1);

	if (!dtypes::has_type(dtype))
	{
		// Unknown type
		conn.async_write_error("Unknown type");
		return;
	}

	try
	{
		server.db_.set_value(
			chBuf,  // hash of the key
			dtype, 
			chBuf + keySize + 1 + sizeof(dtypes::dtype_t), // address of the space where value is stored
			pmsg.size - keySize - 1 - sizeof(dtypes::dtype_t) // size of this space
		);
	}
	catch (dtypes::unsufficient_space exc)
	{
		// Not enough space for actual data
		conn.async_write_error("Not enough space for VALUE");
		return;
	}
	
	server.apply_all_connections(
		boost::bind(static_cast<void(*)(tcp_connection&, asio_server&, const char*)>(&get_handler::send_value), boost::placeholders::_1, boost::ref(server), chBuf)
	);
}

void asio_server::handlers::delete_handler::operator()(tcp_connection& conn, asio_server& server, processed_message& msgp)
{
	char* key = msgp.as_cstr();
	size_t keyHash = make_hash(key);
	server.logger_->debug("Received GET from {}, {}", keyHash, key);

	bool deleted = server.db_.delete_key(keyHash);

	if (!deleted)
		get_handler::send_key_not_found(conn, key);
	else
		server.apply_all_connections(
			boost::bind(&get_handler::send_key_not_found, boost::placeholders::_1, key)
		);
}


void asio_server::handlers::get_all_handler::operator()(tcp_connection& conn, asio_server& server, message_preamble&)
{
	server.db_.iterate(
		[&conn, &server](size_t&, db_object & obj)
		{
			get_handler::send_value(conn, server, obj.get_key().data());
		}
	);
}

_END_STATEDB_NET

