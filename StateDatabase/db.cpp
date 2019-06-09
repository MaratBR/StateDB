#include "pch.h"
#include "db.h"
#include "segment_container.h"
#include "hash.h"

_BEGIN_STATEDB

void _STATEDB db_inner::write_to(_STATEDB_UTILS abstract_ostream& o)
{
	write_object(header, o);
}

void _STATEDB db_inner::read_from(_STATEDB_UTILS abstract_istream& i)
{
	read_object(&header, i);
}

size_t _STATEDB db_inner::get_size() const
{
	return header.get_size();
}

boost::optional<db_object*> db::get_object(size_t keyHash)
{
	db_object* obj = inner->tree->get(keyHash);
	if (obj == nullptr)
		return boost::none;
	return boost::optional<db_object*>(obj);
}

void db::set_value(const char* key, dtypes::dtype_t dt, void* src, size_t len)
{
	db_object obj(dt, key);
	obj.load_from(src, len);
	size_t keyHash = make_hash(key);
	inner->tree->set(keyHash, obj);
}

bool db::delete_key(db_key_type keyHash)
{
	return inner->tree->delete_key(keyHash);
}

void db::iterate(boost::function<void(size_t&, db_object&)> fn)
{
	inner->tree->iterate(fn);
}

db::numeric_operation_result db::add(size_t keyHash, int32_t val)
{
	boost::optional<db_object*> obj = get_object(keyHash);
	if (!obj.has_value())
		return db::NumericOpNotFound;
	if (!obj.value()->is_numeric())
		return db::NumericOpNotNumeric;
	
	return db::NumericOpDone;
}


_END_STATEDB
