#pragma once
#include "pch.h"

_BEGIN_STATEDB_NET

// Remove padding for messages
#pragma pack(push, 1)

struct hello_message_static
{
	char _HELLO[5] { 'H', 'e', 'L', 'l', 'O' };
	uint8_t protocol_version;

	bool valid(uint8_t proto_version) const;
};


struct message_preamble
{
	const char _PREAMBLE[3]{ 'M', 'S', 'G' };
	commands::command_t id;
	uint32_t size;


	bool valid() const;
};

struct message_out_preamble
{
	commands::command_t id;
	uint32_t size;
};

template<commands::command_t ID>
struct static_simple_message : public message_out_preamble
{
	static_simple_message()
	{
		id = ID;
	}
};

using make_pong_message = static_simple_message<commands::response_pong>;

#pragma pack(pop)


struct processed_message
{
	processed_message(const commands::command_t id, const uint32_t size, void* buffer);

	const commands::command_t id;
	const uint32_t size;
	void* buffer;
};

_END_STATEDB_NET