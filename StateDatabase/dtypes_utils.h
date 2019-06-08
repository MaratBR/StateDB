#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

std::pair<char*, size_t> get_string_mxsz(char* src, size_t srcSize)
{
	size_t sz = 0;
	while (sz < srcSize && src[sz] != '\0')
	{
		sz++;
	}

	if (sz == srcSize)
	{
		// Given binary data does not contain zero-byte at the end, so we have to add it
		src[srcSize - 1] = '\0';
		sz = srcSize - 1;
	}

	return { src, sz };
}

_END_STATEDB_UTILS