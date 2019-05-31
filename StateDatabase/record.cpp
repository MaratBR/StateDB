#include "pch.h"
#include "record.h"

_BEGIN_STATEDB

dtypes::dtype_e db_record::get_type() const
{
	return type;
}

void db_record::set(void* mem, size_t srcLen)
{
	memcpy_s(data.data(), data.size(), mem, srcLen);
}

void db_record::set_type(dtypes::dtype_e type_)
{
	if (type_ == type)
		return;

	type = type_;
	data.resize(size());
}

void db_record::set(dtypes::dtype_e dtype, void* mem, size_t srcLen)
{
	set_type(dtype);
	set(mem, srcLen);
}

size_t db_record::size() const
{
	return dtypes::dynamic_types.find(type) == dtypes::dynamic_types.end() ? dtypes::static_sizes.at(type) : 0;
}

_END_STATEDB