#pragma once
#include <boost/asio.hpp>
#include "connection.h"

using namespace boost;
using namespace asio::ip;

namespace statedb {
	namespace net {
		class asio_server
		{
		public:
			asio_server(tcp::endpoint ep);

			void listen();
			bool is_listening() const;
		private:
			void start_accept();	
			void handle_accept(tcp_connection::pointer connection, const boost::system::error_code& ec);

			asio::io_service io_service_;
			tcp::acceptor acceptor_;
			bool is_listening_ = false;
		};
	}
}
