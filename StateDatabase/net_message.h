#pragma once
#include "pch.h"
#include "flags.h"

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
	using flags_type = flags_t<1>;
	commands::command_t id;
	flags_type flags;
	uint32_t size;
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

using make_key_deleted_preamble = static_simple_message<commands::response_key_deleted>;

using make_key_updated_preamble = static_simple_message<commands::response_key_update>;

using make_error_preamble = static_simple_message<commands::response_error>;

#pragma pack(pop)


struct processed_message
{
	processed_message(message_preamble& msgp, void* buffer);

	const commands::command_t id;
	uint32_t size;
	message_preamble::flags_type flags;
	void* buffer;

	char* get_cstr();
	char* as_cstr();
};

_END_STATEDB_NET