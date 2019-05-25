#include "stdafx.h"
#include "utils.h"

void statedb::utils::insert_data_fstream(void* data, size_t size, std::fstream& fs, size_t oldSize)
{

}

void statedb::utils::shift_stream(std::fstream& fs, int offset)
{
	if (offset == 0) return;

	if (offset < 0)
	{
		
	}
}

void statedb::utils::write_object_generic(void* ptr, size_t size, abstract_ostream& o)
{
	o.write(reinterpret_cast<byte_t*>(ptr), size);
}

void statedb::utils::read_object_generic(void* dest, size_t size, abstract_istream& i)
{
	i.read(reinterpret_cast<char*>(dest), size);
}
