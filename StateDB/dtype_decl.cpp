#include "stdafx.h"
#include "dtype_decl.h"

using namespace statedb;
using namespace statedb::utils;

void statedb::dtype_decl::write_to(abstract_ostream& o)
{
	write_object(id, o);
	write_object(_flags, o);
	write_object(human_name, o);

	if (!is_dynamic)
	{
		auto length = fields.size();
		write_object(length, o);
		for (field_t& col : fields)
		{
			write_object(col, o);
		}
	}
}

void statedb::dtype_decl::read_from(abstract_istream& i)
{
	read_object(&id, i);
	read_object(&_flags, i);
	read_object(&human_name, i);

	is_dynamic = _flags.get<0>();

	if (!is_dynamic)
	{
		// Read sequence of types ID's 
		size_t length;
		read_object(&length, i);
		field_t col;
		for (size_t ii = 0; ii < length; ii++)
		{
			read_object(&col, i);
			fields.push_back(col);
		}
	}
	else
	{
		// Read max_length
		read_object(&max_length, i);
	}
}

size_t statedb::dtype_decl::get_size() const
{
	return sizeof(id) + sizeof(_flags) + human_name.size + (is_dynamic ? sizeof(max_length) : (sizeof(fields.size()) + fields.size() * sizeof(fields[0])));
}
