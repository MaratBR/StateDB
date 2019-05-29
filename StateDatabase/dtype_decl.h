#pragma once
#include "pch.h"
#include "flags.h"
#include "basic_types.h"


_BEGIN_STATEDB

using dtype_id_t = uint16_t;
using field_t = dtype_id_t;

struct dtype_decl : public utils::stream_rw<dtype_decl>
{
	dtype_id_t id;
	flags_t<2> _flags;
	bool is_dynamic;
#	define UNDEFINED_DTYPE_SIZE SIZE_MAX    // data type can have any size
	size_t max_length; // for dynamic types only
	std::vector<field_t> fields;
	char_arr<30> human_name;

	// Унаследовано через stream_rw
	virtual void write_to(_STATEDB_UTILS abstract_ostream& o) override;
	virtual void read_from(_STATEDB_UTILS abstract_istream& i) override;
	virtual size_t get_size() const override;
};

_END_STATEDB