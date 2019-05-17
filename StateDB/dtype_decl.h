#pragma once
#include "flags.h"

using namespace std;

namespace statedb {
	using dtype_id_t = uint16_t;
	using field_t = dtype_id_t;

	struct dtype_decl : public utils::stream_rw<dtype_decl>
	{
		dtype_id_t id;
		flags_t<2> _flags;
		bool is_dynamic;
#		define UNDEFINED_DTYPE_SIZE SIZE_MAX    // data type can have any size
		size_t max_length; // for dynamic types only
		vector<field_t> fields;
		char* human_name;

		// Унаследовано через stream_rw
		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
	};
}