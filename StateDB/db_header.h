#pragma once
#include "stdafx.h"

namespace statedb {
#	define DB_MAGIC "StAtE1"
#	define DB_VERSION 1
#	define MINIMAL_HEADER_SIZE (sizeof(::statedb::db_header))

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

	struct db_pointers
	{
		size_t dtypes;
		size_t info;
		size_t data;
	};

	struct db_meta
	{
		const byte_t version = DB_VERSION;
		db_pointers pointers;
	};

	struct db_info
	{
		char * name;
		char * description;
	};

	WRITE_OBJECT_SPECIALIZATION(db_info)
	{
		_Stream << _Val.name << _Val.description;
	}

	READ_OBJECT_SPECIALIZATION(db_info)
	{
		_Stream >> _Dest->name >> _Dest->description;
	}

	struct db_header
	{
		db_header(){}
		static const size_t magic_offset = sizeof(db_magic);
		bool save_db_hash;
		size_t db_hash;
		db_meta meta;

		db_info info;
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