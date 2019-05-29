#include "pch_terminal.h"
#include <asio_server.h>
#include <locale>

using namespace std;

#define _TRY try {
#define _CRITICAL_CATCH } \
catch(::statedb::db_exception exc) { \
	spdlog::critical(exc.what()); \
	exit(1); \
}

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



class Server
{
public:
	Server()
	{
		boost::asio::deadline_timer dt(io, boost::posix_time::seconds(10));
		dt.async_wait([](const boost::system::error_code&) { std::cout << "HI"; });
		io.run();
	}

private:
	boost::asio::io_context io;
};

int main()
{
	setlocale(LC_ALL, "Russian");
	spdlog::set_level(spdlog::level::debug);
	show_header();

	statedb::net::asio_server server(boost::asio::ip::tcp::endpoint(
		boost::asio::ip::address_v4::loopback(), 3456));

	server.start_listening();
	server.run();
	//time_t rawtime;
	//tm timeinfo;
	//char* buffer = new char[strlen(STATEDB_DEFAULT_DB_FILE) + 80];
	//memcpy_s(buffer, strlen(STATEDB_DEFAULT_DB_FILE), STATEDB_DEFAULT_DB_FILE, strlen(STATEDB_DEFAULT_DB_FILE));

	//time(&rawtime);
	//localtime_s(&timeinfo, &rawtime);
	//strftime(buffer + strlen(STATEDB_DEFAULT_DB_FILE), 80, "%d-%m-%YT%H-%M-%S", &timeinfo);

	//delete db;

	//fs::copy_file(STATEDB_DEFAULT_DB_FILE, buffer);
	//fs::remove(fs::current_path() / STATEDB_DEFAULT_DB_FILE);
}