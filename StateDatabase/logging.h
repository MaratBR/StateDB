#pragma once
#include "pch.h"

_BEGIN_STATEDB

namespace logging 
{
#define STATEDB_LOGGING_CUSTOM_PREFIX "CUSTOM"
#define STATEDB_LOGGING_CONNECTION_PREFIX "CONN"
#define STATEDB_LOGGING_DRIVER_PREFIX "DB_DRIVER"
#define STATEDB_LOGGING_GLOBAL_LOGGER "G"
#define STATEDB_LOGGING_SERVER_PREFIX "SERVER"

	inline std::shared_ptr<spdlog::logger> _construct_logger(std::string name)
	{
		auto logger = spdlog::stdout_color_mt(name);

		static const char* pattern = "%^%L%$ [%b %d, %Y %H:%M:%S.%e] [thread %t] " _D("(%@) ") "[%n] %v";
		logger->set_pattern(pattern);

		return logger;
	}

	inline std::shared_ptr<spdlog::logger> get_logger(std::string name)
	{
		name = boost::str(boost::format("StDB/%s") % name);
		std::shared_ptr<spdlog::logger> logger = spdlog::get(name);
		if (logger == nullptr)
		{
			logger = _construct_logger(name);
			
		}
		return logger;
	}

	inline std::shared_ptr<spdlog::logger> get_logger_with_prefix(std::string prefix, std::string name)
	{
		return get_logger(
			boost::str(boost::format("%s/%s") % prefix % name)
		);
	}

	inline std::shared_ptr<spdlog::logger> get_connection_logger(const std::string& name)
	{
		return get_logger_with_prefix(STATEDB_LOGGING_CONNECTION_PREFIX, name);
	}

	inline std::shared_ptr<spdlog::logger> get_driver_logger(const std::string& dbName)
	{
		return get_logger_with_prefix(STATEDB_LOGGING_DRIVER_PREFIX, dbName);
	}

	inline std::shared_ptr<spdlog::logger> get_server_logger(const std::string& addr)
	{
		return get_logger_with_prefix(STATEDB_LOGGING_SERVER_PREFIX, addr);
	}

	inline std::shared_ptr<spdlog::logger> get_global_logger()
	{
		return get_logger(STATEDB_LOGGING_GLOBAL_LOGGER);
	}
}

_END_STATEDB