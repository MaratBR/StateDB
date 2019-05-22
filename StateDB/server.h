#pragma once
#include "stdafx.h"
#include "exceptions.h"

#ifdef _WIN32
#	include <WinSock2.h>
#endif

#define STATEDB_BACKLOG 128

namespace statedb {
	namespace net {
		namespace globals {
#ifdef _WIN32
			WSADATA WSA;
			int WSA_result;
			bool WSA_ready = false;
			char* WSA_msg = nullptr;
#define STATEDB_NET_LAST_ERROR_CODE (::statedb::net::globals::WSA_result)
#else
#define STATEDB_NET_LAST_ERROR_CODE 0
#endif

			void init()
			{
#ifdef _WIN32
				if (WSA_ready) return;
				WSA_result = WSAStartup(MAKEWORD(2, 2), &WSA);
				assert_zero(WSA_result, "Failed to init WSA");
				WSA_ready = true;
#endif
			}
		}

		class db_server
		{
		public:
			db_server(uint16_t port, const char* host);

			void init();
			void listen();
			bool is_listening();

			int get_last_error() const
			{
#ifdef _WIN32
				return WIN_last_result;
#endif
			}
		private:
			void _init();
			uint16_t port;
			const char* host;
			bool is_initialized = false;
			bool _is_listening = false;

#ifdef _WIN32
			SOCKADDR_IN listen_addr, client_addr;
			SOCKET _socket = INVALID_SOCKET, client_socket = INVALID_SOCKET;
			int WIN_last_result = 0;
			int client_addr_sz = sizeof(client_addr);
#endif
		};
	}
}