#include "pch.h"
#include <StateDB/db_wrapper.h>

using namespace std;

#define _TRY try {
#define _CRITICAL_CATCH } \
catch(::statedb::db_exception exc) { \
	spdlog::critical(exc.what()); \
	exit(1); \
}

struct {
	fs::path working_dir;
} settings;

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

statedb::db_wrapper* db;

void init_db()
{
	spdlog::set_level(spdlog::level::debug);
	settings.working_dir = fs::current_path();
	db = new statedb::db_wrapper(settings.working_dir);

	_TRY
		db->load_data();
	_CRITICAL_CATCH
}

enum options_index
{
	UNKNOWN,
	PORT,
	HOST,
	THREADS_COUNT,
	LOGGING_LEVEL
};

int main()
{
	show_header();
	init_db();
}