#pragma once
#include "pch.h"

_BEGIN_STATEDB_NET

struct hello_message_static
{
	char _HELLO[5] { 'H', 'e', 'L', 'l', 'O' };
	uint8_t protocol_version;

	bool valid(uint8_t proto_version) const;
};

struct message_preamble
{
	char _PREAMBLE[9] { 'D', 'o' };
	commands::command_t id;

	bool valid() const;
};

struct indetity_message 
{
	bool has_identity = false;
	char* login = nullptr;
	char* password = nullptr;
};

_END_STATEDB_NET