#pragma once
#include "pch.h"
#include "commands.h"
#include "connection.h"

_BEGIN_STATEDB_NET

class messages_dispatcher
{
public:
	using dispatcher_function = boost::function<void(tcp_connection&)>;

	messages_dispatcher();

	void get_dispatcher(commands::command_t id);
private:
	void init_registry();

	void on_get_info(tcp_connection& conn);
	void on_get_version(tcp_connection& conn);
	void on_get_dtypes(tcp_connection& conn);
	void on_get_keys(tcp_connection& conn);

	void on_get_val(tcp_connection& conn);
	void on_set_val(tcp_connection& conn);
	void on_delete_key(tcp_connection& conn);
	void on_rename_key(tcp_connection& conn);
	void on_force_update_key(tcp_connection& conn);

	void on_create_dtype(tcp_connection& conn);
	void on_drop_dtype(tcp_connection& conn);

	void on_ping(tcp_connection& conn);
	void on_pong(tcp_connection& conn);
	void on_transaction_start(tcp_connection& conn);

	commands::cmd_handlers_registry cmd_registry;
};

_END_STATEDB_NET