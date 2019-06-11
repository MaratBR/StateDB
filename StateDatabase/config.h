#pragma once

// DB settings

#ifndef STATEDB_CHANGES_INCLUDE_VALUE
#	define STATEDB_CHANGES_INCLUDE_VALUE false
#endif

#ifndef CHECK_UNIMPLEMENTED_CMD_CODES
#	define CHECK_UNIMPLEMENTED_CMD_CODES true
#endif

#ifndef IGNORE_MAJOR_VERSION_MISMATCH
#define IGNORE_MAJOR_VERSION_MISMATCH true
#endif


#ifndef STATEDB_NET_TRASH_BUF_SIZE
#	define STATEDB_NET_TRASH_BUF_SIZE 256 // The size of the buffer for discarding data from TCP socket (net_utils.h)
#endif

#ifndef STATEDB_BLOCK_SIZE
#	define STATEDB_BLOCK_SIZE 1024
#endif

#ifndef STATEDB_MAX_BLOCK_SIZE
#define STATEDB_MAX_BLOCK_SIZE (1024*1024*256)
#endif

#ifndef STATEDB_NET_TIMEOUT
#define STATEDB_NET_TIMEOUT ::boost::posix_time::milliseconds(6000)
#endif

#define STATEDB_SERVER_STATS_TASK_TIMEOUT ::boost::posix_time::minutes(2)
#define STATEDB_MAX_OPERATIONS_PENDING 256

#ifndef STATEDB_MAX_FSTREAM_BUFFER_SIZE
#define STATEDB_MAX_FSTREAM_BUFFER_SIZE 1024*1024 // 1M buffer for inserting in the middle
#endif

#define STATEDB_INIT_CONTAINER_VAR(name, obj) ::statedb::segment_container<decltype(obj)> name(obj, STATEDB_BLOCK_SIZE)

#define STATEDB_ZEROES 0
#define STATEDB_ONES (~0)
#define STATEDB_EMPTY_SPACE_BYTE STATEDB_ZEROES
#define STATEDB_DEFAULT_DB_FILE _DD("storage-D.statedb", "storage.statedb")

#include "commands.h"

#define STATEDB_PROTOCOL_VERSION 1
