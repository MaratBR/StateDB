#include "stdafx.h"
#include "utils.h"

void statedb::utils::write_object_generic(void * ptr, size_t size, std::ostream & o)
{
	o.write(reinterpret_cast<char*>(ptr), size);
}

void statedb::utils::read_object_generic(void * dest, size_t size, std::istream & i)
{
	i.read(reinterpret_cast<char*>(dest), size);
}
