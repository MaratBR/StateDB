﻿#include "pch_terminal.h"
#include <asio_server.h>
#include <db_wrapper.h>

#include <locale>
#include <filesystem>
#include <iostream>
using namespace std;
#define _TRY try {
#define _CRITICAL_CATCH } catch(::statedb::db_exception exc) { spdlog::critical(exc.what()); exit(1); }

void show_header()
{
	cout
		<< "   _____ __        __       _____ ____ " << endl
		<< "  / ___// /_____ _/ /____  / __  / __ )" << endl
		<< "  \\__ \\/ __/ __ `/ __/ _ \\/ / / / __  |" << endl
		<< " ___/ / /_/ /_/ / /_/  __/ /_/ / /_/ / " << endl
		<< "/____/\\__/\\__,_/\\__/\\___/_____/_____/  " << endl
		<< endl
		<< "COMPILED WITH DEBUG: " << _DD("yes", "no") << endl
		<< "VERSION: " << STATEDB_VERSION << endl
		<< "CREDITS:" << endl
		<< STATEDB_CREDITS
		<< endl;
}
std::shared_ptr<spdlog::logger> global_logger;
void prepare()
{
	global_logger = statedb::logging::get_global_logger();
	SetConsoleOutputCP(CP_UTF8);
#ifdef DEBUG
	spdlog::set_level(spdlog::level::debug);
#endif
}

int main()
{
	prepare();
	show_header();
	statedb::db_wrapper dbw(boost::filesystem::current_path());
	_TRY
		dbw.load_data();
	_CRITICAL_CATCH
	statedb::net::asio_server server(boost::asio::ip::tcp::endpoint(
		boost::asio::ip::address_v4::loopback(), 3456), dbw.get_inner().value());

	server.start_listening();
	server.run();
}