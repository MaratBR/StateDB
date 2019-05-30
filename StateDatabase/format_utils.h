#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

// Converts chrono system_clock's duration type to string, based on https://stackoverflow.com/questions/42138599/how-to-format-stdchrono-durations 
std::string format_duration(std::chrono::system_clock::duration dur);

_END_STATEDB_UTILS