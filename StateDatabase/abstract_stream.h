#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

class abstract_stream_base
{
public:
	enum seek_mode
	{
		non_seekable,
		seekable,
		forward_seekable,
	};
};

using seek_mode_e = abstract_stream_base::seek_mode;

class abstract_ostream : public abstract_stream_base
{
public:
	virtual void write(byte_t* src, size_t srcLen) = 0;
	virtual std::streampos tellp() = 0;
	virtual abstract_ostream& seekp(std::streampos pos) = 0;
	virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) = 0;
	virtual seek_mode_e get_seek_modep() const { return seek_mode_e::non_seekable; }
};

class abstract_istream : public abstract_stream_base
{
public:
	virtual void read(byte_t* dest, size_t count) = 0;
	virtual std::streampos tellg() = 0;
	virtual abstract_istream& seekg(std::streampos pos) = 0;
	virtual abstract_istream& seekg(std::streamoff off, std::ios_base::seekdir way) = 0;
	virtual seek_mode_e get_seek_modeg() const { return seek_mode_e::non_seekable; }
};

class istream_wrp : public abstract_istream
{
public:
	istream_wrp(std::istream& is);
	virtual void read(byte_t* dest, size_t count) override;
	virtual std::streampos tellg() override;
	virtual abstract_istream& seekg(std::streampos pos) override;
	virtual abstract_istream& seekg(std::streamoff off, std::ios_base::seekdir way) override;
private:
	std::istream& is;

};

class ostream_wrp : public abstract_ostream
{
public:
	ostream_wrp(std::ostream& os);
	virtual void write(byte_t* src, size_t srcLen) override;
	virtual std::streampos tellp() override;
	virtual abstract_ostream& seekp(std::streampos pos) override;
	virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) override;
private:
	std::ostream& os;
};

_END_STATEDB_UTILS