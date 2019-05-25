#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/system/error_code.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "config.h"
#include "utils.h"

using boost::asio::ip::tcp;
using boost::system::error_code;

namespace statedb {
	namespace net {
		namespace utils {
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
			void async_skip_impl(tcp::socket& s, int n, boost::function<void(error_code const&)> h,
				char* buf, error_code const& ec, std::size_t bytes_transferred)
			{
				assert(bytes_transferred <= n);
				n -= bytes_transferred;
				if (ec || n == 0) {
					delete[] buf;
					h(ec);
					return;
				}

				s.async_read_some(boost::asio::buffer(buf, std::min(STATEDB_NET_TRASH_BUF_SIZE, n)),
					boost::bind(&async_skip_impl, boost::ref(s), n, h, buf, _1, _2));
			}

			void async_skip_bytes(tcp::socket& s, int n, boost::function<void(error_code const&)> h)
			{
				size_t size_ = std::min(STATEDB_NET_TRASH_BUF_SIZE, n);
				char* buf = new char[size_];
				s.async_read_some(boost::asio::buffer(buf, size_),
					boost::bind(&async_skip_impl, boost::ref(s), n, h, buf, _1, _2));
			}
		
			size_t skip_bytes(tcp::socket& s, int n)
			{
				size_t size_ = std::min(STATEDB_NET_TRASH_BUF_SIZE, n);
				char* buf = new char[size_];
				return s.read_some(boost::asio::buffer(buf, size_));
			}
		}
	}
}