#include "stdafx.h"
#include "dtype_decl.h"

void statedb::dtype_decl::write_to(std::ostream& o)
{
	utils::write_object(id, o);
	utils::write_object(_flags, o);
	o << human_name;

	if (!is_dynamic)
	{
		auto length = fields.size();
		utils::write_object(length, o);
		for (field_t& col : fields)
		{
			utils::write_object(col, o);
		}
	}
}

void statedb::dtype_decl::read_from(std::istream& i)
{
	utils::read_object(&id, i);
	utils::read_object(&_flags, i);
	i >> human_name;

	is_dynamic = _flags.get<0>();

	if (!is_dynamic)
	{
		// Read sequence of types ID's 
		size_t length;
		utils::read_object(&length, i);
		field_t col;
		for (size_t ii = 0; ii < length; ii++)
		{
			utils::read_object(&col, i);
			fields.push_back(col);
		}
	}
	else
	{
		// Read max_length
		utils::read_object(&max_length, i);
	}
}

size_t statedb::dtype_decl::get_size() const
{
	return sizeof(id) + sizeof(_flags) + sizeof(char) * (strlen(human_name) + 1) + (is_dynamic ? sizeof(max_length) : (sizeof(fields.size()) + fields.size() * sizeof(fields[0])));
}
