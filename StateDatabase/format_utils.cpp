#include "pch.h"
#include "format_utils.h"

_BEGIN_STATEDB_UTILS 

std::string format_duration(std::chrono::system_clock::duration dur)
{
	auto dur_ = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
	auto c(dur_.count());
	std::ostringstream oss;
	oss << std::setfill('0') // set field fill character to '0'
		<< (c / 3600000 / 24) // format days
		<< std::setw(0) << ":" << std::setw(2)
		<< (c / 3600000) % 24 // format hours
		<< std::setw(0) << ":" << std::setw(2)
		<< (c / 60000) % 60 // format minutes
		<< std::setw(0) << ":" << std::setw(2)
		<< (c / 1000) % 60 // format seconds
		<< std::setw(0) << ":" << std::setw(3)
		<< c % 1000; // format milliseconds
	return oss.str();
}

_END_STATEDB_UTILS
