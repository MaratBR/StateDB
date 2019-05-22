#include "stdafx.h"
#include "net_message.h"

constexpr statedb::net::message_t::type statedb::net::message_t::get_msg_type() const
{
	return static_cast<type>(GET_CMD_TYPE(id));
}

constexpr command_t statedb::net::message_t::get_inner_type() const
{
	return EXTRACT_INNER(id, get_msg_type());
}
