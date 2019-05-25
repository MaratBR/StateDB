#include "stdafx.h"
#include "server.h"

statedb::net::db_server::db_server(uint16_t port, const char* host)
{
}

void statedb::net::db_server::init()
{
	if (is_initialized) return;
	is_initialized = true;

	_init();
}

void statedb::net::db_server::listen()
{
	if (_is_listening) return;
	_is_listening = true;
	while (_is_listening)
	{
#ifdef _WIN32
		client_addr_sz = sizeof(client_addr);
		client_socket = accept(_socket, reinterpret_cast<sockaddr*>(&client_socket), &client_addr_sz);
		if (client_socket == INVALID_SOCKET)
		{
			WIN_last_result = WSAGetLastError();
			throw db_exception("Server: ::accept() failed");
		}

		char* strBuf = new char[16];
		spdlog::debug("Accepted new connection from {} on port {}", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, strBuf, 16), htons(client_addr.sin_port));
		
#endif
	}
}

bool statedb::net::db_server::is_listening()
{
	return _is_listening;
}

void statedb::net::db_server::_init()
{
#ifdef _WIN32
	// Create address
	spdlog::info("Server protocol: TCP");
	spdlog::debug("Creating SOCKADDR_IN structure...");

	listen_addr.sin_family = AF_INET;

	ULONG addr;
	INT result;

	if (host == nullptr)
		addr = INADDR_ANY;
	else if ((result = inet_pton(AF_INET, host, &addr)) != 1)
	{
		if (result == -1)
		{
			throw db_exception("Server: invalid IP address");
		}
		else
		{
			throw db_exception("Server: WSA error occur");
		}
	}
	 
	listen_addr.sin_addr.s_addr = (host == nullptr) ? INADDR_ANY : addr;
	listen_addr.sin_port = htons(port);

	// Init socket

	spdlog::debug("Initializing socket...");
	_socket = socket(listen_addr.sin_family, SOCK_STREAM, 0);

	if (_socket == INVALID_SOCKET)
	{
		throw db_exception("Server: socket() failed");
	}

	spdlog::debug("Binding to {}:{} ...", host, port);
	if ((WIN_last_result = bind(_socket, reinterpret_cast<sockaddr*>(&listen_addr), sizeof(listen_addr))) == SOCKET_ERROR)
	{
		throw db_exception("Server: bind() failed");
	}


	if ((WIN_last_result = ::listen(_socket, STATEDB_BACKLOG)) == SOCKET_ERROR)
	{
		throw db_exception("Server: ::listen() failed");
	}
#endif
}
