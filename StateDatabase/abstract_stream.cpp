#include "pch.h"
#include "abstract_stream.h"

using namespace statedb::utils;

istream_wrp::istream_wrp(std::istream& is)
	: is(is)
{
}

void istream_wrp::read(byte_t* dest, size_t count)
{
	is.read(dest, count);
}

std::streampos istream_wrp::tellg()
{
	return is.tellg();
}

abstract_istream& statedb::utils::istream_wrp::seekg(std::streampos pos)
{
	is.seekg(pos);
	return *this;
}

abstract_istream& statedb::utils::istream_wrp::seekg(std::streamoff off, std::ios_base::seekdir way)
{
	is.seekg(off, way);
	return *this;
}

ostream_wrp::ostream_wrp(std::ostream& os)
	: os(os)
{
}

void ostream_wrp::write(byte_t* src, size_t srcLen)
{
	os.write(src, srcLen);
}

std::streampos ostream_wrp::tellp()
{
	return os.tellp();
}

abstract_ostream& ostream_wrp::seekp(std::streampos pos)
{
	os.seekp(pos);
	return *this;
}

abstract_ostream& ostream_wrp::seekp(std::streamoff off, std::ios_base::seekdir way)
{
	os.seekp(off, way);
	return *this;
}
