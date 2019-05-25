#pragma once

#define STATEDB_CHANGES_TRACING_MODE_ALWAYS		2	// Always notify clients about new value
#define STATEDB_CHANGES_TRACING_MODE_DEMAND		1	// Don't notify but support CACHE_CHECK_CMD
#define STATEDB_CHANGES_TRACING_MODE_DISABLED	0	// Don't notify and don't support CACHE_CHECK_CMD

#define STATEDB_CHANGES_INCLUDE_VALUE false
#define CHECK_UNIMPLEMENTED_CMD_CODES true
#define IGNORE_MAJOR_VERSION_MISMATCH true 

#define STATEDB_NET_TRASH_BUF_SIZE 256 // The size of the buffer for discarding data from TCP socket (net_utils.h)

#define BLOCK_SIZE 1024
#define ASIO_BUF_STREAM_BLOCK_SIZE 128
#define INIT_CONTAINER(name, obj) ::statedb::segment_container name(obj, BLOCK_SIZE)
#define MAX_FSTREAM_BUFFER_SIZE 1024*1024 // 1M buffer for inserting in the middle

#define MAX_BLOCK_SIZE (1024*1024*256)
#define ZEROES 0
#define ONES (~0)
#define EMPTY_SPACE_BYTE ZEROES
#define DEFAULT_DB_FILE _DD("storage-D.statedb", "storage.statedb")

using command_t = uint16_t; // Don't change this

#define _CMD(x) (command_t)(x)

#define _CMDTYPE_MASK _CMD(3 << 14)

#define CMD_TYPE_META 0b000 // Getting metadata from DB
#define CMD_TYPE_DATA 0b001 // CRUD operations
#define CMD_TYPE_BEHV 0b010 // Commands that changes how DB should behave, what it should do and when
#define CMD_TYPE_STRUCT 0b011 // Creating new data types
#define CMD_TYPE_SERVICE 0b100 // Service commands such as transactions, ping, pong
#define CMD_TYPE_RESPONSE 0b111


#define CMD_TYPE_BITS 3
#define CMD_ID_BITS (16 - CMD_TYPE_BITS)

#define MAKE_CMD(type, innerCode) _CMD(((type) << CMD_ID_BITS) | (innerCode))
#define GET_CMD_TYPE(cmd) (((cmd) & _CMDTYPE_MASK) >> CMD_ID_BITS)
#define EXTRACT_INNER(cmd, cmdType) ((cmd) xor ((cmdType) << CMD_ID_BITS))

#pragma region Reponse messages IDs

#define RESPONSE_KEY_UPDATE MAKE_CMD( CMD_TYPE_RESPONSE, 0 )
#define RESPONSE_KEY_DELETED MAKE_CMD( CMD_TYPE_RESPONSE, 1 )
#define RESPONSE_KEY_FORCE_LOGOUT MAKE_CMD( CMD_TYPE_RESPONSE, 2 )
#define RESPONSE_KEY_ERROR MAKE_CMD( CMD_TYPE_RESPONSE, 14 ) /// 114 == 0xE

#pragma endregion

// It's critical to have these commands numerated from 0 to n without unused code numbers

using cmd_handler_ptr = void*;

struct cmd_handlers_registry
{
	cmd_handler_ptr operator[](command_t cmd)
	{
		auto cmdType = GET_CMD_TYPE(cmd);

		size_t offset = offsets[cmdType];
		command_t inner = EXTRACT_INNER(cmd, cmdType);
		size_t ptr = reinterpret_cast<size_t>(this) + offset + inner;

		if (ptr >= reinterpret_cast<size_t>(this) + offsets[cmdType + 1])
		{
			return nullptr;
		}

		return reinterpret_cast<cmd_handler_ptr>(ptr);
	}

#pragma region Meta commands

	struct meta_commands 
	{
#		define CMD_GET_INFO		MAKE_CMD( CMD_TYPE_META, 0 )
		cmd_handler_ptr			get_info = nullptr;

#		define CMD_GET_VERSION	MAKE_CMD( CMD_TYPE_META, 1 )
		cmd_handler_ptr			get_version = nullptr;

#		define CMD_GET_DTYPES	MAKE_CMD( CMD_TYPE_META, 2 )
		cmd_handler_ptr			get_dtypes = nullptr;

#		define CMD_GET_KEYS		MAKE_CMD( CMD_TYPE_META, 3 )
		cmd_handler_ptr			get_keys = nullptr;

#		define CMD_GET_STATS	MAKE_CMD( CMD_TYPE_META, 4 )
		cmd_handler_ptr			get_stats = nullptr;
	} _meta;

#pragma endregion

#pragma region Data commands

	struct data_commands 
	{
#		define CMD_GET_VAL MAKE_CMD( CMD_TYPE_DATA, 0 )
		cmd_handler_ptr get_val = nullptr;

#		define CMD_SET_VAL MAKE_CMD( CMD_TYPE_DATA, 1 )
		cmd_handler_ptr set_val = nullptr;

#		define CMD_DELETE_KEY MAKE_CMD( CMD_TYPE_DATA, 2 )
		cmd_handler_ptr delete_key = nullptr;

#		define CMD_RENAME_KEY MAKE_CMD( CMD_TYPE_DATA, 3 )
		cmd_handler_ptr rename_key = nullptr;
	
#		define CMD_FORCE_UPDATE_KEY MAKE_CMD( CMD_TYPE_DATA, 4 )
		cmd_handler_ptr force_update_key = nullptr;
	} _data;

#pragma endregion

#pragma region Behaviour commands
	struct behv_commands 
	{
	} _behv;
#pragma endregion

#pragma region Structure commands that modify database structure, also this type is used for any other command
	struct struct_commands
	{
#		define CMD_CREATE_DTYPE MAKE_CMD( CMD_TYPE_STRUCT, 0 )
		cmd_handler_ptr create_dtype = nullptr;
#		define CMD_DROP_DTYPE MAKE_CMD( CMD_TYPE_STRUCT, 1 )
		cmd_handler_ptr drop_dtype = nullptr;
	} _struct;
#pragma endregion

#pragma region Service commands
	struct service_commands
	{
#		define CMD_PING MAKE_CMD( CMD_TYPE_SERVICE, 0 )
		cmd_handler_ptr ping = nullptr;
#		define CMD_PONG MAKE_CMD( CMD_TYPE_SERVICE, 1 )
		cmd_handler_ptr pong = nullptr;
#		define CMD_TRANSACTION MAKE_CMD( CMD_TYPE_SERVICE, 2 )
		cmd_handler_ptr transaction_start = nullptr;
	} _service;
#pragma endregion

	static const size_t offsets[6];
};

constexpr size_t meta_handlers_offset = offsetof(cmd_handlers_registry, _meta);
constexpr size_t data_handlers_offset = offsetof(cmd_handlers_registry, _data);
constexpr size_t behv_handlers_offset = offsetof(cmd_handlers_registry, _behv);
constexpr size_t struct_handlers_offset = offsetof(cmd_handlers_registry, _struct);
constexpr size_t service_handlers_offset = offsetof(cmd_handlers_registry, _service);


#ifndef STATEDB_CHANGES_TRACING_MODE
#	define STATEDB_CHANGES_TRACING_MODE STATEDB_CHANGES_TRACING_MODE_ALWAYS
#endif

#define DB_INT16_TYPE 1
#define DB_BYTE_TYPE 2
#define DB_INT32_TYPE 3
#define DB_DYNAMIC 4

#define USER_DTYPE_LOW 256


