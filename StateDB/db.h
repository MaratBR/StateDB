#pragma once
#include "bpt.h"
#include "record.h"
#include "db_header.h"
#include "dtypes_collection.h"
#include "exceptions.h"

namespace statedb {
	using db_key_type = size_t;

	const char* const ERR_DESCRIPTION_INVALID_MAGIC = "Invalid magic value";

	struct db_inner : public utils::stream_rw<db_inner>
	{
		bpt::tree<db_key_type, db_record, 6>* tree = nullptr;
		db_header header;
		dtypes_collection dtypes;

		db_inner() {}
		~db_inner()
		{
			if (tree)
			{
				delete tree;
				tree = nullptr;
			}
		}

		void debug_print();

		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
		virtual size_t get_size() const override;
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
			inner->debug_print();
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

		// Resets inner DB object to default state
		void generate_inner()
		{
			reset_inner();

			inner->header.set_defaults();
			spdlog::info("DB object has been reseted");
		}

		// Loads data from DB file
		//
		// THROWS:
		//		* statedb::db_exception
		//			throw by verify_magic() method when file has invalid magic sequence
		void _load()
		{
			verify_magic();

			utils::read_object(&inner->header, fstream);

			inner->debug_print();
		}

		std::fstream& fstream;
		
		db_inner* inner;
	};
}