#pragma once
#include "pch.h"
#include <exception>

_BEGIN_STATEDB

class db_exception : public std::exception
{
public:
	db_exception() {}
	db_exception(const char* const str) : std::exception(str) {}
};

_END_STATEDB