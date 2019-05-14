#pragma once

// Debug stuff

#define _FALLBACK_DEBUG false
#ifndef DEBUG
#	ifdef _DEBUG
#		define DEBUG _DEBUG
#	else
#		define DEBUG _FALLBACK_DEBUG
#	endif
#endif


#if DEBUG
#	define _D(x) x
#else
#	define _D(x)
#endif

#if DEBUG
#	define _DD(debug, release) debug
#else
#	define _DD(debug, release) release
#endif

#define _LOG(x)  do { _STD cerr << (x); } while(0)
#define DEBUG_LOG(x) _D( _LOG(x) )


#if DEBUG
#	include <iostream>
constexpr void print_depth(size_t depth)
{
	while (depth-- > 0) _STD cout << '\t';
}
#endif

#define CONCAT_COMMA(...) __VA_ARGS__

// Some useful (or not) types

using none_t = struct{};
constexpr none_t none = none_t();
using byte_t = unsigned char;


// https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
// This is not required anymore, but I'll keep that just yet 
//#define STATIC_SIGNATURE_CHECKER(name, return_type, ...)							\
//template<typename T>																\
//class has_sig_ ## name																\
//{																					\
//	template<typename U, return_type(U::*)(__VA_ARGS__) const> struct SFINAE {};	\
//	template<typename U> static char _test(SFINAE<U, &U::name>*);					\
//	template<typename U> static int _test(...);										\
//public:																				\
//	static const bool value = sizeof(_test<T>(0)) == sizeof(char);					\
//};