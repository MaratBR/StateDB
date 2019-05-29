#include "pch.h"
#include "net_message.h"

_BEGIN_STATEDB_NET

bool hello_message_static::valid(uint8_t proto_version)
{
	return (
		protocol_version == proto_version &&
		_HELLO[0] == 'H' &&
		_HELLO[1] == 'e'&&
		_HELLO[2] == 'L'&&
		_HELLO[3] == 'l'&&
		_HELLO[4] == 'O'
		);
}

_END_STATEDB_NET