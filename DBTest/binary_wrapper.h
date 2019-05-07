#pragma once
#include "stdafx.h"

namespace statedb {
	namespace utils {
		template <typename T>
		void write_object(std::ostream & out, T & val)
		{
			BYTE * ptr = reinterpret_cast<BYTE*>(&val);
			for (size_t i = 0; i < sizeof(T); i++)
			{
				out << ptr[i];
			}
		}
	}
}