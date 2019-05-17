#pragma once
#include "stdafx.h"


namespace statedb {
	namespace log_utils {
		std::string get_tab(size_t count) { return std::string(count, '\t'); }
	}
}