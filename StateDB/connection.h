#pragma once
#include "stdafx.h"
#include <boost/asio.hpp>
#include "asio_stream.h"

namespace gutils = statedb::utils;
using namespace boost;
using asio::ip::tcp;

namespace statedb {
	namespace net {
		class tcp_connection
		{
		public:
			using pointer = std::shared_ptr<tcp_connection>;

			static tcp_connection::pointer create(asio::io_context& io_context_)
			{
				return pointer(new tcp_connection(io_context_));
			}
			tcp::socket& socket();

			void start();
			tcp_connection(asio::io_context& io_context_)
				: socket_(io_context_)
			{
			}

			tcp::socket socket_;
		};
	}
}