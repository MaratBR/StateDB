#pragma once
#include "pch.h"
#include "asio_stream.h"
#include "dynamic_storage.h"
#include "logging.h"
#include "net_utils.h"


_BEGIN_STATEDB_NET

class tcp_connection
{
public:
	using pointer = std::shared_ptr<tcp_connection>;

	static tcp_connection::pointer create(std::shared_ptr<boost::asio::io_context> io_context_)
	{
		return pointer(new tcp_connection(io_context_));
	}

	ASIO_SOCKET& socket();

	void start();

	void close();

	void init();
private:

	void received_hello(const BOOST_ERR_CODE& ec, size_t br);

	void timer_expired(const BOOST_ERR_CODE& ec);

	void timer_expired_fatal(const BOOST_ERR_CODE& ec);

	inline void timeout_close()
	{
		spdlog::info("{} - Connection closed due to timeout expiration", address_string);
		close();
	}

	tcp_connection(std::shared_ptr<boost::asio::io_context> io_context_);

	template<typename T>
	void async_read_message(
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		boost::function<void(const BOOST_ERR_CODE&)> th,
		std::chrono::milliseconds timeout = std::chrono::milliseconds::zero()
	)
	{
		static const char* typename_ = TYPE_NAME(T);
		spdlog::debug("{} - reserved for {} ({} bytes)", address_string, typename_, sizeof(T));
		auto buf = dynamic_storage_.allocate_asio_buffer<T>();
		if (timeout.count() == 0)
		{
			socket_.async_read_some(buf, h);
		}
		_STATEDB_UTILS read_with_timeout(
			socket_,
			buf,
			HELLO_TIMEOUT,
			h,
			th,
			deadline_timer_
		);
	}

	void _async_read_message_or_close__read(boost::function<void(const BOOST_ERR_CODE&, size_t)> h, const BOOST_ERR_CODE& ec, size_t bt);

	void _async_read_message_or_close__timeout(const BOOST_ERR_CODE& ec);

	template<typename U>
	inline void async_read_message_or_close(
		boost::function<void(const BOOST_ERR_CODE&, size_t)> h,
		std::chrono::milliseconds timeout = 0ms
	)
	{
		async_read_message<U>(
			boost::bind(&tcp_connection::_async_read_message_or_close__read, this, h, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred),
			boost::bind(&tcp_connection::_async_read_message_or_close__timeout, this, boost::asio::placeholders::error),
			timeout
			);
	}

	boost::posix_time::milliseconds HELLO_TIMEOUT = STATEDB_NET_HELLO_TIMEOUT;
	boost::posix_time::milliseconds PING_TIMEOUT = STATEDB_NET_PING_TIMEOUT;
	boost::asio::ip::tcp::socket socket_;
	std::shared_ptr<boost::asio::io_context> io_context_;
	std::unique_ptr<boost::asio::deadline_timer> deadline_timer_ = nullptr;
	std::string timed_out_action = "";
	std::string address_string;
	std::shared_ptr<spdlog::logger> logger = nullptr;

	_STATEDB_UTILS dynamic_storage dynamic_storage_;
};

_END_STATEDB_NET