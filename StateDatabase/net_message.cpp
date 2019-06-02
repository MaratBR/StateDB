#include "pch.h"
#include "net_message.h"

_BEGIN_STATEDB_NET

bool hello_message_static::valid(uint8_t proto_version) const
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


bool message_preamble::valid() const
{
	return _PREAMBLE[0] == 'M' && _PREAMBLE[1] == 'S' && _PREAMBLE[2] == 'G';
}

processed_message::processed_message(const commands::command_t id, const uint32_t size, void* buffer) :
	buffer(buffer), id(id), size(size)
{
}


_END_STATEDB_NET
