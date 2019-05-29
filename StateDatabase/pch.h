#ifndef PCH_H
#define PCH_H
#pragma once

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include "framework.h"

#include <istream>
#include <ostream>
#include <fstream>

#include <typeinfo>
#include <vector>
#include <memory>

#include <boost/filesystem.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

//#include "spdlog/fmt/fmt.h"
//#include "spdlog/fmt/bin_to_hex.h"
//#include "spdlog/fmt/ostr.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>

#include "globals.h"
#include "config.h"

#endif //PCH_H

