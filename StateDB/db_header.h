#pragma once
#include "stdafx.h"
#include "exceptions.h"
#include "basic_types.h"

namespace statedb {
#	define DB_MAGIC "StAtE1"
#	define MINIMAL_HEADER_SIZE (sizeof(::statedb::db_header))
#	define _SAVE_DB_HASH_DEFAULT false

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
		db_string name = nullptr;
		db_string description = nullptr;

		void dispose()
		{
			description.dispose();
			name.dispose();
		}

		void set_defaults()
		{
			dispose();

			description.c_str = new char[strlen(DEFAULT_DB_DESCRIPTION) + 1];
			name.c_str = new char[strlen(DEFAULT_DB_NAME) + 1];
			assert_zero(strcpy_s(name.c_str, strlen(DEFAULT_DB_NAME) + 1, DEFAULT_DB_NAME), "Failed to copy default DB name");
			assert_zero(strcpy_s(description.c_str, strlen(DEFAULT_DB_DESCRIPTION) + 1, DEFAULT_DB_DESCRIPTION), "Failed to copy default DB description");
		}

		// Унаследовано через stream_rw
		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
		virtual size_t get_size() const override;
	};


	struct db_header : public utils::stream_rw<db_header>
	{
		db_header(){}
		static const size_t magic_offset = sizeof(db_magic);
		bool save_db_hash = _SAVE_DB_HASH_DEFAULT;
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
		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
		virtual size_t get_size() const override;
	};
}

// https://gist.github.com/codebrainz/8ece2a9015a3ed0d260f
namespace std
{
	template <>
	struct hash<char*>
	{
		size_t operator()(const char *s) const
		{
			// http://www.cse.yorku.ca/~oz/hash.html
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};
}