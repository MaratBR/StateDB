#include "pch.h"
#include "object.h"

_BEGIN_STATEDB


db_object::db_object(const std::string& key)
	: db_object(STATEDB_DTYPE_NONE, key)
{
}

db_object::db_object(dtypes::dtype_t dtype, const std::string& key)
	: dtype(dtype), key(key)
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

size_t db_object::get_size()
{
	return get_implementor().get_size(ds);
}

size_t db_object::write_to(void* to, size_t bufSize)
{
	return get_implementor().load_to(to, bufSize, ds);
}

size_t db_object::load_from(void* from, size_t bufSize)
{
	return get_implementor().load_from(from, bufSize, ds);
}

std::string& db_object::get_key()
{
	return key;
}

_END_STATEDB

