#pragma once
#include "pch.h"
#include "abstract_stream.h"

_BEGIN_STATEDB_UTILS

class asio_base_stream
{
public:
	explicit asio_base_stream(ASIO_SOCKET& socket_);

protected:
	ASIO_SOCKET& socket_;
	std::streampos pos = 0;
};

class asio_istream : public asio_base_stream, public abstract_istream
{
public:
	using asio_base_stream::asio_base_stream;

	virtual void read(byte_t* dest, size_t count) override;
	virtual std::streampos tellg() override;
	virtual abstract_istream& seekg(std::streampos pos) override;
	virtual abstract_istream& seekg(std::streamoff off, std::ios_base::seekdir way) override;
	virtual seek_mode_e get_seek_modeg() const override { return utils::seek_mode_e::forward_seekable; }
};

class asio_ostream : public asio_base_stream, public abstract_ostream
{
public:
	using asio_base_stream::asio_base_stream;

	virtual void write(byte_t* src, size_t srcLen) override;
	virtual std::streampos tellp() override;
	virtual abstract_ostream& seekp(std::streampos pos) override;
	virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) override;
	virtual seek_mode_e get_seek_modep() const override { return seek_mode_e::forward_seekable; }
};

class asio_buf_ostream : public abstract_ostream, public asio_base_stream
{
public:
	using asio_base_stream::asio_base_stream;
	using buffer_t = std::vector<char>;

	virtual void write(byte_t* src, size_t srcLen) override;
	virtual std::streampos tellp() override;
	virtual abstract_ostream& seekp(std::streampos pos) override;
	virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) override;
	virtual seek_mode_e get_seek_modep() const override { return seek_mode_e::forward_seekable; }

	void discard() { assert(!locked()); buffer.clear(); }
	size_t flush_to(ASIO_SOCKET& socket_);
	void async_flush_to(ASIO_SOCKET& socket_, boost::function<void(const boost::system::error_code&, size_t)> h);
	bool locked() const { return locked_; }
private:
	constexpr void lock() { locked_ = true; }
	constexpr void unlock() { locked_ = false; }

	buffer_t buffer;
	bool locked_ = false;
};

_END_STATEDB_UTILS