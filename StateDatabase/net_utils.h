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
	char* buf, const BOOST_ERR_CODE& ec, std::size_t bytes_transferred);

void async_skip_bytes(ASIO_SOCKET& s, int n, boost::function<void(const BOOST_ERR_CODE&)> h);

size_t skip_bytes(ASIO_SOCKET& s, int n);

_END_STATEDB_UTILS