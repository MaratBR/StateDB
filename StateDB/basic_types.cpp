#include "stdafx.h"
#include "basic_types.h"

statedb::db_string::~db_string()
{
	dispose();
}

void statedb::db_string::dispose()
{
	_ENSUREDEL(c_str);
	c_str = nullptr;
}

void statedb::db_string::write_to(std::ostream& o)
{
	auto len = strlen(c_str);
	utils::write_object(len, o);
	o.write(c_str, len);
}

void statedb::db_string::read_from(std::istream& i)
{
	size_t len = 0;
	utils::read_object(&len, i);
	_ENSUREDEL(c_str);
	c_str = new char[len + 1];
	c_str[len] = '\0';
	i.read(c_str, len);
}

size_t statedb::db_string::get_size() const
{
	return strlen(c_str) + sizeof(strlen(0));
}
