#pragma once

#include "targetver.h"
#include <ostream>
#include <istream>
#include <functional>
#include <vector>
#include <filesystem>
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bin_to_hex.h"

#define WIN32_LEAN_AND_MEAN


#include "globals.h"
#include "config.h"
#include "features_impl.h"
#include "utils.h"
#include "hash.h"
#include "exceptions.h"
