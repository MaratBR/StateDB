#pragma once
#include "db.h"

namespace fs = std::filesystem;

namespace statedb {
	const char* const ERR_DESCRIPTION_CANT_OPEN_DB_FILE = "Failed to open database file";

	class db_wrapper
	{
	public:
		db_wrapper(const fs::path workingDirectory, const std::string dbFilename = "storage.statedb")
			: db_filepath(workingDirectory / dbFilename),
			db_filename(dbFilename)
		{
			if (fs::create_directories(workingDirectory))
			{
				spdlog::debug("Directory {} was created", workingDirectory.string());
			}

			auto* ptr = this;
			spdlog::debug("New instance of db object has been initialized workingDirectory={} file={} ptr={:x}", workingDirectory.string(), dbFilename, (size_t)this);
		}

		void load_data()
		{
			fstream = open_file();

			if (!fstream->good())
			{
				spdlog::error(ERR_DESCRIPTION_CANT_OPEN_DB_FILE);
				throw statedb::db_exception(ERR_DESCRIPTION_CANT_OPEN_DB_FILE);
			}

			spdlog::debug("Database file is open now");
			verify_file();
		}
	private:
		void verify_file()
		{
			spdlog::debug("Verifying file...");
			uintmax_t size = fs::file_size(db_filepath);
			spdlog::debug("File size = {}", size);

			
			if (size < MINIMAL_HEADER_SIZE)
			{
				spdlog::info("Database file is empty or was corrupted, rewriting file");
				clear_file();
				_db = new db(*fstream);
				_db->make_fresh();
			}
			else
			{
				_db = new db(*fstream);
				_db->reload();
			}
		}

		void clear_file()
		{
			// Close. Clear. Reopen.
			fstream->close();
			delete fstream;
			fstream = new std::fstream(db_filepath, std::ios::in | std::ios::trunc);
			fstream->close();
			delete fstream;
			fstream = open_file();
		}

		std::fstream* open_file()
		{
			int mode = std::ios::in | std::ios::out | std::ios::binary;
			if (fs::exists(db_filepath))
			{

			}
			else
			{
				mode |= std::ios::trunc;
			}
			return new std::fstream(db_filepath, mode);
		}

		const fs::path db_filepath;
		const std::string db_filename;
		std::fstream* fstream;
		db* _db;
	};
}