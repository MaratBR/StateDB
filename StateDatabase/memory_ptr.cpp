#include "pch.h"
#include "memory_ptr.h"

_BEGIN_STATEDB_UTILS

void* memory_ptr::raw()
{
	return data;
}

_END_STATEDB_UTILS