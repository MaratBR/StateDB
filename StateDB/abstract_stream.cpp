#include "stdafx.h"
#include "abstract_stream.h"

statedb::utils::istream_wrp::istream_wrp(std::istream& is)
	: is(is)
{
}

void statedb::utils::istream_wrp::read(byte_t* dest, size_t count)
{
	is.read(dest, count);
}

statedb::utils::ostream_wrp::ostream_wrp(std::ostream& os)
	: os(os)
{
}

void statedb::utils::ostream_wrp::write(byte_t* src, size_t srcLen)
{
	os.write(src, srcLen);
}
