#include "pch.h"

_BEGIN_STATEDB_UTILS

void async_skip_impl(
	ASIO_SOCKET& s, 
	int n, 
	boost::function<void(const BOOST_ERR_CODE&)> h, 
	char* buf, 
	const BOOST_ERR_CODE& ec, 
	std::size_t bytes_transferred
)
{
	assert(bytes_transferred <= n);
	n -= bytes_transferred;
	if (ec || n == 0) {
		delete[] buf;
		h(ec);
		return;
	}

	s.async_read_some(boost::asio::buffer(buf, std::min(STATEDB_NET_TRASH_BUF_SIZE, n)),
		boost::bind(&async_skip_impl, boost::ref(s), n, h,
			buf, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void async_skip_bytes(ASIO_SOCKET& s, int n, boost::function<void(const BOOST_ERR_CODE&)> h)
{
	size_t size_ = std::min(STATEDB_NET_TRASH_BUF_SIZE, n);
	char* buf = new char[size_];
	s.async_read_some(boost::asio::buffer(buf, size_),
		boost::bind(async_skip_impl, boost::ref(s), n, h,
			buf, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

size_t skip_bytes(ASIO_SOCKET& s, int n)
{
	size_t size_ = std::min(STATEDB_NET_TRASH_BUF_SIZE, n);
	char* buf = new char[size_];
	return s.read_some(boost::asio::buffer(buf, size_));
}


_END_STATEDB_UTILS
