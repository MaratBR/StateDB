#pragma once


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
#	define _D(x) /*** - debug only code (excluded) - ***/
#endif

#if DEBUG
#	define _DD(x, y) x
#else
#	define _DD(x, y) y
#endif

#define _LOG(x)  do { ::std::cerr << (x); } while(0)
#define DEBUG_LOG(x) _D( _LOG(x) )

#if DEBUG
#	include <iostream>
constexpr void print_depth(size_t depth)
{
	while (depth-- > 0) std::cout << '\t';
}
#endif

// B+ tree implementation config
#define BPT_ITERATION true

