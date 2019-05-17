#pragma once
#include "utils.h"

namespace statedb {
	struct db_string : public utils::stream_rw<db_string>
	{
		char* c_str = nullptr;

		db_string(nullptr_t){}
		db_string(char* str) : c_str(str) {}
		
		~db_string();
		void dispose();

		bool operator==(nullptr_t) { return c_str == nullptr; }
		bool operator!=(nullptr_t) { return c_str != nullptr; }

		// Унаследовано через stream_rw
		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
		virtual size_t get_size() const override;


		template<typename OStream>
		friend OStream& operator<<(OStream& os, const db_string& c)
		{
			return os << c.c_str;
		}
	};
}