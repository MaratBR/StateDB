#pragma once
#include <exception>

namespace statedb {
	class db_exception : public std::exception
	{
	public:
		db_exception() {}
		db_exception(const char* const str) : std::exception(str) {}
	};
}