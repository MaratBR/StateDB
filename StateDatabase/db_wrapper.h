#pragma once
#include "pch.h"
#include "db.h"
#include "logging.h"

_BEGIN_STATEDB

namespace fs = boost::filesystem;

const char* const ERR_DESCRIPTION_CANT_OPEN_DB_FILE = "Failed to open database file";

class db_wrapper
{
public:
	db_wrapper(const fs::path workingDirectory, const std::string dbFilename = STATEDB_DEFAULT_DB_FILE)
		: db_filepath(workingDirectory / dbFilename),
		db_filename(dbFilename),
		logger(logging::get_logger_with_prefix("DBw", dbFilename))
	{
		if (fs::create_directories(workingDirectory))
		{
			logger->info("Directory {} was created", workingDirectory.string());
		}

		auto* ptr = this;
		logger->debug("New instance of db wrapper has been initialized workingDirectory={} file={} ptr={:x}", workingDirectory.string(), dbFilename, (size_t)this);
	}

	~db_wrapper()
	{
		dispose();
	}

	boost::optional<db&> get_inner()
	{
		if (_db == nullptr)
			return boost::none;
		return boost::optional<db&>(*_db);
	}

	void dispose()
	{
		_ENSUREDEL(_db);
		if (fstream)
		{
			fstream->clear();
			fstream->close();
			delete fstream;
		}
	}

	void load_data()
	{
		dispose();
		fstream = open_file();

		logger->debug("Database file is open now");
		verify_file();
	}

	void resize_file(size_t newSize)
	{
		close_file();
		fs::resize_file(db_filepath, newSize);
		fstream->open(db_filepath);
	}
private:
	void verify_file()
	{
		logger->info("Verifying file...");
		uintmax_t size = fs::file_size(db_filepath);
		logger->debug("File size = {}", size);

			
		if (size < MINIMAL_HEADER_SIZE)
		{
			logger->info("Database file is empty or was corrupted, rewriting file");
			clear_file();
			_db = new db(*fstream, db_filename);
			_db->make_fresh();
		}
		else
		{
			_db = new db(*fstream, db_filename);
			_db->reload();
			logger->info("DB info loaded");
		}
	}

	void clear_file()
	{
		// Close. Clear. Reopen.
		close_file();
		delete fstream;
		fstream = new fs::fstream(db_filepath, std::ios::in | std::ios::trunc);
		fstream->close();
		delete fstream;
		fstream = open_file();
	}

	void close_file()
	{
		if (fstream && fstream->is_open())
		{
			fstream->clear();
			fstream->close();
		}
	}

	fs::fstream* open_file()
	{
		int mode = std::ios::in | std::ios::out | std::ios::binary;
		if (fs::exists(db_filepath))
		{

		}
		else
		{
			mode |= std::ios::trunc;
		}
		auto* fs_ = new fs::fstream(db_filepath, mode);

		if (!fs_->good())
		{
			spdlog::error(ERR_DESCRIPTION_CANT_OPEN_DB_FILE);
			throw statedb::db_exception(ERR_DESCRIPTION_CANT_OPEN_DB_FILE);
		}

		return fs_;
	}

	const fs::path db_filepath;
	const std::string db_filename;
	fs::fstream* fstream = nullptr;
	db* _db = nullptr;
	std::shared_ptr<spdlog::logger> logger;
};

_END_STATEDB