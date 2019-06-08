#pragma once
#include "pch.h"

_BEGIN_STATEDB

template<size_t count>
struct flags_t
{
	static constexpr size_t bytes_count = count / 8 + 1;
	byte_t data[bytes_count];

	template<size_t index>
	bool get() const
	{
		static_assert(index < count, "Index is out of range");
		size_t byteIndex = index / 8;

		return data[byteIndex] & (1 << (7 - (index - 8 * byteIndex)));
	}

	template<size_t index>
	void set(bool val)
	{
		static_assert(index < count, "Index is out of range");
		size_t byteIndex = index / 8;

		if (val)
		{
			data[byteIndex] = data[byteIndex] | (1 << (7 - (index - 8 * byteIndex)));
		}
		else
		{
			data[byteIndex] = data[byteIndex] & ~(1 << (7 - (index - 8 * byteIndex)));
		}
	}
};

_END_STATEDB
