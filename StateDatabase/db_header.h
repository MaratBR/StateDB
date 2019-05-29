#pragma once
#include "stdafx.h"
#include "exceptions.h"
#include "basic_types.h"

_BEGIN_STATEDB

#define DB_MAGIC "StAtE1"
#define MINIMAL_HEADER_SIZE (sizeof(::statedb::db_header))
#define _SAVE_DB_HASH_DEFAULT false

struct db_magic 
{ 
	const char value[7] = DB_MAGIC; 

	bool verify()
	{
		for (int i = 0; i < 7; i++)
			if (value[i] != DB_MAGIC[i])
				return false;

		return true;
	}
};

struct db_meta
{
	byte_t version = STATEDB_MAJOR_VERSION;

	void set_defaults()
	{
		version = STATEDB_MAJOR_VERSION;
	}
};

const char* const DEFAULT_DB_DESCRIPTION = "A STATE database (" STATEDB_VERSION ")";
const char* const DEFAULT_DB_NAME = "DB";

struct db_info : public utils::stream_rw<db_info>
{
	~db_info() { dispose(); }
	char_arr<32> name = nullptr;
	char_arr<256> description = nullptr;

	void dispose()
	{
	}

	void set_defaults()
	{
		description = DEFAULT_DB_DESCRIPTION;
		name = DEFAULT_DB_NAME;
	}

	// Унаследовано через stream_rw
	virtual void write_to(_STATEDB_UTILS abstract_ostream& o) override;
	virtual void read_from(_STATEDB_UTILS abstract_istream& i) override;
	virtual size_t get_size() const override;
};


struct db_header : public utils::stream_rw<db_header>
{
	db_header(){}
	static const size_t magic_offset = sizeof(db_magic);
	bool save_db_hash = _SAVE_DB_HASH_DEFAULT;
	bool debug_mode = DEBUG_BOOL;
	size_t db_hash = 0;
	db_meta meta;

	db_info info;

	void set_defaults()
	{
		db_hash = 0;
		save_db_hash = _SAVE_DB_HASH_DEFAULT;
			
		info.set_defaults();
		meta.set_defaults();
	}

	// Унаследовано через stream_rw
	virtual void write_to(_STATEDB_UTILS abstract_ostream& o) override;
	virtual void read_from(_STATEDB_UTILS abstract_istream& i) override;
	virtual size_t get_size() const override;
};

_END_STATEDB
