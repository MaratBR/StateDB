#include "pch.h"
#include "object.h"

_BEGIN_STATEDB


db_object::db_object()
	: db_object(STATEDB_DTYPE_NONE)
{
}

db_object::db_object(dtypes::dtype_t dtype)
	: dtype(dtype)
{
	clear();
}


void db_object::set_type(dtypes::dtype_t dtype)
{
	if (dtype == this->dtype)
		return;
	if (!dtypes::has_type(dtype))
		throw invalid_dtype(dtype);

	this->dtype = dtype;
	clear();
}

void db_object::clear()
{
	get_implementor().init(ds);
}

dtypes::dtype_t db_object::get_type() const
{
	return dtype;
}

_END_STATEDB

