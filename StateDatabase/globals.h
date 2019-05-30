#pragma once
#include "compile_time.h" // ???

// Debug stuff

#define _FALLBACK_DEBUG false
#ifndef DEBUG
#	ifdef _DEBUG
#		define DEBUG _DEBUG
#	else
#		define DEBUG _FALLBACK_DEBUG
#	endif
#endif

#ifdef DEBUG
#	define DEBUG_BOOL true
#else
#	define DEBUG_BOOL false
#endif


#if DEBUG
#	define _D(x) x
#else
#	define _D(x)
#endif

#define _REMOVE_ME(x) _D(x)

#if DEBUG
#	define _DD(debug, release) debug
#else
#	define _DD(debug, release) release
#endif
#define _NOTHING

#define _LOG(x)  do { _STD cerr << (x); } while(0)
#define DEBUG_LOG(x) _D( _LOG(x) )


#if DEBUG
#	include <iostream>
#	define print_depth(x) 
#endif

#define CONCAT_COMMA(...) __VA_ARGS__

// https://stackoverflow.com/questions/14396139/how-do-i-concatenate-two-macros-with-a-dot-between-them-without-inserting-spaces
#define M_CONC(A, B) M_CONC_(A, B)
#define M_CONC_(A, B) A##B

// Stringify macro
#define STR(x) _STRSTR(x)
#define _STRSTR(x) _STRSTR2(x)
#define _STRSTR2(x) _STRSTR3(x)
#define _STRSTR3(x) #x

// Chck that pointer is not nullptr 
#define _ENSUREDEL(x) do { if ((x) != nullptr) { delete(x); } } while(0)

// Check non zero value
#define assert_zero(x, msg) do { if ((x) != 0) { throw ::statedb::db_exception((msg)); } } while(0)

// Some useful (or not) types
using byte_t = char;
#define BOOST_ERR_CODE ::boost::system::error_code
#define ASIO_SOCKET boost::asio::ip::tcp::socket

// Library structure
#define _STATEDB statedb::
#define _STATEDB_NET M_CONC(_STATEDB, net::)
#define _STATEDB_UTILS M_CONC(_STATEDB, utils::)
#define _STATEDB_BPT M_CONC(_STATEDB, bpt::)

#define _BEGIN_STATEDB namespace statedb {
#define _END_STATEDB }

#define _BEGIN_STATEDB_UTILS _BEGIN_STATEDB namespace utils {
#define _END_STATEDB_UTILS } _END_STATEDB

#define _BEGIN_STATEDB_NET _BEGIN_STATEDB namespace net {
#define _END_STATEDB_NET } _END_STATEDB

#define _BEGIN_STATEDB_BPT _BEGIN_STATEDB namespace bpt {
#define _END_STATEDB_BPT } _END_STATEDB

#define TYPE_NAME(t) typeid(t).name()
#define TYPE_NAME_VAL(val) TYPE_NAME(decltype(val))
#define TYPE_NAME_PTR(ptr) TYPE_NAME_VAL(*ptr)
#define TYPE_NAME_THIS TYPE_NAME_PTR(this)



