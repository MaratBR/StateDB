#include "pch.h"
#include "messages_dispatcher.h"

_BEGIN_STATEDB_NET

messages_dispatcher::messages_dispatcher()
{
	init_registry();
}

void messages_dispatcher::init_registry()
{
#	define _HNDLR_PTR(name) nullptr//&commands::cmd_handler(boost::bind(&messages_dispatcher::name, this, std::placeholders::_1))

	cmd_registry._meta.c0_get_info = _HNDLR_PTR(on_get_info);
	cmd_registry._meta.c1_get_version = _HNDLR_PTR(on_get_version);
	cmd_registry._meta.c2_get_dtypes = _HNDLR_PTR(on_get_dtypes);
	cmd_registry._meta.c3_get_keys = _HNDLR_PTR(on_get_keys);

	cmd_registry._data.c0_get_val = _HNDLR_PTR(on_get_val);
	cmd_registry._data.c1_set_val = _HNDLR_PTR(on_set_val);
	cmd_registry._data.c2_delete_key = _HNDLR_PTR(on_delete_key);
	cmd_registry._data.c3_rename_key = _HNDLR_PTR(on_rename_key);
	cmd_registry._data.c4_force_update_key = _HNDLR_PTR(on_force_update_key);

	// cmd_registry._behv

	cmd_registry._struct.c0_create_dtype = _HNDLR_PTR(on_create_dtype);
	cmd_registry._struct.c1_drop_dtype = _HNDLR_PTR(on_drop_dtype);

	cmd_registry._service.c0_ping = _HNDLR_PTR(on_ping);
	cmd_registry._service.c1_pong = _HNDLR_PTR(on_pong);
	cmd_registry._service.c2_transaction_start = _HNDLR_PTR(on_transaction_start);

#	undef _HNDLR_PTR
}

void messages_dispatcher::on_get_info(tcp_connection& conn)
{
}

void messages_dispatcher::on_get_version(tcp_connection& conn)
{
}

void messages_dispatcher::on_get_dtypes(tcp_connection& conn)
{
}

void messages_dispatcher::on_get_keys(tcp_connection& conn)
{
}

void messages_dispatcher::on_get_val(tcp_connection& conn)
{
}

void messages_dispatcher::on_set_val(tcp_connection& conn)
{
}

void messages_dispatcher::on_delete_key(tcp_connection& conn)
{
}

void messages_dispatcher::on_rename_key(tcp_connection& conn)
{
}

void messages_dispatcher::on_force_update_key(tcp_connection& conn)
{
}

void messages_dispatcher::on_create_dtype(tcp_connection& conn)
{
}

void messages_dispatcher::on_drop_dtype(tcp_connection& conn)
{
}

void messages_dispatcher::on_ping(tcp_connection& conn)
{
}

void messages_dispatcher::on_pong(tcp_connection& conn)
{
}

void messages_dispatcher::on_transaction_start(tcp_connection& conn)
{
}

_END_STATEDB_NET