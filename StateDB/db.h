#pragma once
#include "stdafx.h"
#include "bpt.h"
#include "record.h"
#include "db_header.h"
#include "utils.h"

namespace statedb {
	using db_key_type = size_t;

	const char* const ERR_DESCRIPTION_INVALID_MAGIC = "Invalid magic value";

	struct db_inner
	{
		bpt::tree<db_key_type, db_record, 6>* tree = nullptr;
		db_header header;

		db_inner() {}
		~db_inner()
		{
			if (tree) delete tree;
		}
	};

	class db
	{
	public:
		db(std::fstream& stream)
			: fstream(stream)
		{
		}

		void make_fresh()
		{
			rewind_stream();

			db_magic magic;
			utils::write_object(magic, fstream);

			generate_inner();
			utils::write_object(*inner, fstream);
		}

		void reload()
		{
			reset_inner();
			rewind_stream();

			_load();
		}

		void verify_magic()
		{
			db_magic magic;
			utils::read_object(&magic, fstream);
			if (!magic.verify())
				throw db_exception(ERR_DESCRIPTION_INVALID_MAGIC);
		}

	public:
		void reset_inner()
		{
			if (inner)
				delete inner;
			inner = new db_inner;
		}

		void rewind_stream()
		{
			fstream.seekg(0, std::ios::beg);
		}

		void generate_inner()
		{
			reset_inner();

			
		}

		void _load()
		{
			verify_magic();

			utils::read_object(&inner->header, fstream);
		}

		std::fstream& fstream;
		
		db_inner* inner;
	};
}