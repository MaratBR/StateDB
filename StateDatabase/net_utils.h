#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

/* skip_impl
	* Implementation function that skips given amount of data (in bytes). Requires buffer,
	* size of the buffer is defined in config.h as STATEDB_NET_TRASH_BUF_SIZE
	* 
	* @param s
	*		Socket to read data from
	* @param n
	*		Count of bytes left
	* @param h
	*		ASIO's handler function
	* @param buf
	*		Buffer for data being read
	* @param ec
	*		Error code if any occur
	* @param bytes_transferred
	*		Count of bytes transferred in the previous step
	*/
void async_skip_impl(ASIO_SOCKET& s, int n, boost::function<void(const BOOST_ERR_CODE&)> h,
	char* buf, const BOOST_ERR_CODE& ec, std::size_t bytes_transferred)
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

template<typename MutableBufferSequence>
void read_with_timeout(
	ASIO_SOCKET& socket_,
	const MutableBufferSequence& buffer, // buffer for read data
	const boost::posix_time::milliseconds dur, // timeout
	boost::function<void(const BOOST_ERR_CODE&, size_t)> h, // read callback
	boost::function<void(const BOOST_ERR_CODE&)> th, // timer callback
	boost::asio::deadline_timer& timer
)
{
	boost::optional<BOOST_ERR_CODE> timer_result;
	boost::optional<BOOST_ERR_CODE> read_result;
	timer.expires_from_now(boost::posix_time::hours(1));

	socket_.async_read_some(buffer, [&timer_result, &timer, &read_result, &h](const BOOST_ERR_CODE & _EC, size_t _S) {
		if (timer_result)
			// Timer has expired
			return;
		timer.cancel();
		read_result = _EC;
		h(_EC, _S); 
		});
	timer.async_wait(
		[&timer_result, &read_result, &th](const BOOST_ERR_CODE & _EC)
		{
			if (read_result)
				// Read operation completed in time
				return;
			timer_result = _EC;
			th(_EC);
		});
}

_END_STATEDB_UTILS