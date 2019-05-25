#pragma once

#include "targetver.h"
#include <ostream>
#include <istream>
#include <functional>
#include <vector>
#include <filesystem>
#include <fstream>
#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/fmt/ostr.h"

#define WIN32_LEAN_AND_MEAN


#include "version.h"
#include "globals.h"
#include "config.h"
#include "utils.h"