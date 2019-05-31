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



private:
	
	std::map<commands::command_t, dispatcher_function> commands;
	dispatcher_function fallback_handler;
};

_END_STATEDB_NET