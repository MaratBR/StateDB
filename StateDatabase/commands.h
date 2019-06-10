#pragma once

_BEGIN_STATEDB
	
#define _CMD(x) (_STATEDB commands::command_t)(x) // converting numeric value to command_t
#define STATEDB_MAKE_RESPONSE_CMD(x) _CMD((0xff << 8) | _CMD(x))
#define STATEDB_MAKE_REQUEST_CMD(x) _CMD((0xff << 8) | _CMD(x))

#define _RSP(x) STATEDB_MAKE_RESPONSE_CMD(x) // temporary shortcut


namespace commands {
	typedef uint16_t command_t; // Don't change this

	constexpr command_t response_key_update = _RSP(1);
	constexpr command_t response_key_deleted = _RSP(2);
	constexpr command_t response_force_logout = _RSP(3);
	constexpr command_t response_pong = _RSP(4);
	constexpr command_t response_error = _RSP(0xEE);

#undef _RSP

	constexpr command_t request_get = 1;
	constexpr command_t request_delete = 2;
	constexpr command_t request_ping = 3;
	constexpr command_t request_set = 4;
	constexpr command_t request_get_all = 5;
}

_END_STATEDB


