#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

// Converts chrono system_clock's duration type to string, based on https://stackoverflow.com/questions/42138599/how-to-format-stdchrono-durations 
std::string format_duration(std::chrono::system_clock::duration dur);

inline std::wstring to_wstring(const char* cstr)
{
	return std::wstring(cstr, cstr + strlen(cstr) - 1);
}

inline std::wstring to_wstring(std::string str)
{
	return std::wstring(str.begin(), str.end());
}

_END_STATEDB_UTILS