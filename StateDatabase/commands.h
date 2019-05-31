#pragma once

_BEGIN_STATEDB
	
namespace commands {
	typedef uint16_t command_t; // Don't change this


	constexpr command_t create_mask(size_t len)
	{
		if (len == 1)
			return 1u;
		else
			return 1u << (len - 1) | create_mask(len - 1);
	}
}


_END_STATEDB

#	define _CMD(x) (_STATEDB commands::command_t)(x) // converting numric value to command_t


#	define CMD_TYPE_META 0b000 // Getting metadata from DB
#	define CMD_TYPE_DATA 0b001 // CRUD operations
#	define CMD_TYPE_BEHV 0b010 // Commands that changes how DB should behave, what it should do and when
#	define CMD_TYPE_STRUCT 0b011 // Creating new data types
#	define CMD_TYPE_SERVICE 0b100 // Service commands such as transactions, ping, pong
#	define CMD_TYPE_RESPONSE 0b111


#	define CMD_TYPE_BITS 3
#	define CMD_ID_BITS (16 - CMD_TYPE_BITS)

#	define _CMDTYPE_MASK _STATEDB commands::create_mask(CMD_TYPE_BITS) << CMD_ID_BITS  // Mask for first
static_assert(_CMDTYPE_MASK == (uint16_t)0b1110000000000000, "asd");

#	define MAKE_CMD(type, innerCode) _CMD(((type) << CMD_ID_BITS) | (innerCode))
#	define GET_CMD_TYPE(cmd) (((cmd) & _CMDTYPE_MASK) >> CMD_ID_BITS)
#	define EXTRACT_INNER(cmd, cmdType) ((cmd) xor ((cmdType) << CMD_ID_BITS))

#pragma region Reponse messages IDs

#	define RESPONSE_KEY_UPDATE MAKE_CMD( CMD_TYPE_RESPONSE, 0 )
#	define RESPONSE_KEY_DELETED MAKE_CMD( CMD_TYPE_RESPONSE, 1 )
#	define RESPONSE_KEY_FORCE_LOGOUT MAKE_CMD( CMD_TYPE_RESPONSE, 2 )
#	define RESPONSE_KEY_ERROR MAKE_CMD( CMD_TYPE_RESPONSE, 14 ) /// 114 == 0xE

#pragma endregion

#pragma region Meta commands

#	define CMD_GET_INFO		MAKE_CMD( CMD_TYPE_META, 0 )
#	define CMD_GET_VERSION	MAKE_CMD( CMD_TYPE_META, 1 )
#	define CMD_GET_DTYPES	MAKE_CMD( CMD_TYPE_META, 2 )
#	define CMD_GET_KEYS		MAKE_CMD( CMD_TYPE_META, 3 )
#	define CMD_GET_STATS	MAKE_CMD( CMD_TYPE_META, 4 )

#pragma endregion

#pragma region Data commands

#	define CMD_GET_VAL MAKE_CMD( CMD_TYPE_DATA, 0 )
#	define CMD_SET_VAL MAKE_CMD( CMD_TYPE_DATA, 1 )
#	define CMD_DELETE_KEY MAKE_CMD( CMD_TYPE_DATA, 2 )
#	define CMD_RENAME_KEY MAKE_CMD( CMD_TYPE_DATA, 3 )
#	define CMD_FORCE_UPDATE_KEY MAKE_CMD( CMD_TYPE_DATA, 4 )

#pragma endregion

#pragma region Behaviour commands

#pragma endregion

#pragma region Structure commands that modify database structure
#	define CMD_CREATE_DTYPE MAKE_CMD( CMD_TYPE_STRUCT, 0 )
#	define CMD_DROP_DTYPE MAKE_CMD( CMD_TYPE_STRUCT, 1 )
#pragma endregion

#pragma region Service commands
#	define CMD_PING MAKE_CMD( CMD_TYPE_SERVICE, 0 )
#	define CMD_PONG MAKE_CMD( CMD_TYPE_SERVICE, 1 )
#	define CMD_TRANSACTION MAKE_CMD( CMD_TYPE_SERVICE, 2 )
#pragma endregion


