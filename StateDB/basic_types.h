#pragma once
#include "utils.h"

namespace statedb {
	template<size_t SIZE>
	struct char_arr
	{
		char_arr(const char* str)
			: char_arr(str, strlen(str))
		{
		}

		char_arr(const char* str, size_t sourceLen)
			: char_arr()
		{
			copy_from(str, sourceLen);
		}

		char_arr<SIZE>& operator=(const char* str)
		{
			copy_from(str);
			return *this;
		}

		char_arr(nullptr_t) : char_arr() {}

		char_arr()
		{
			memset(data, 0, SIZE);
		}

		void copy_from(const char* str)
		{
			copy_from(str, strlen(str));
		}

		void copy_from(const char* str, size_t sourceLen)
		{
			assert_zero(
				memcpy_s(data, SIZE, str, sourceLen),
				"Failed to copy data from raw string to inner container"
			);
		}

		static const size_t size = SIZE;
		char data[SIZE];

		template<typename TStream>
		friend TStream& operator<<(TStream& s, const char_arr<SIZE>& arr) { return s << (static_cast<const char*>(arr.data)); }
	};
}