#pragma once
#include "stdafx.h"
#include "abstract_stream.h"
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;
namespace gutils = statedb::utils;

namespace statedb {
	namespace net {
		class asio_base_stream
		{
		public:
			asio_base_stream(tcp::socket& socket_);

		protected:
			tcp::socket& socket_;
			std::streampos pos = 0;
		};

		class asio_istream : public asio_base_stream, public gutils::abstract_istream
		{
		public:
			using asio_base_stream::asio_base_stream;

			virtual void read(byte_t* dest, size_t count) override;
			virtual std::streampos tellg() override;
			virtual abstract_istream& seekg(std::streampos pos) override;
			virtual abstract_istream& seekg(std::streamoff off, std::ios_base::seekdir way) override;
			virtual gutils::seek_mode_e get_seek_modeg() const override { return utils::seek_mode_e::forward_seekable; }
		};

		/* asio_ostream - Simple synchronous stream that immediately sends data to
		 * the socket.
		 */
		class asio_ostream : public asio_base_stream, public gutils::abstract_ostream
		{
		public:
			using asio_base_stream::asio_base_stream;

			virtual void write(byte_t* src, size_t srcLen) override;
			virtual std::streampos tellp() override;
			virtual abstract_ostream& seekp(std::streampos pos) override;
			virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) override;
			virtual gutils::seek_mode_e get_seek_modep() const override { return gutils::seek_mode_e::forward_seekable; }
		};
	
		/* asio_buf_ostream - bufferized stream that writes data to the buffer and 
		 * allows to flush the buffer to the socket in one go
		 */
		class asio_buf_ostream : public gutils::abstract_ostream, public asio_base_stream
		{
		public:
			using asio_base_stream::asio_base_stream;
			using buffer_t = std::vector<char>;

			virtual void write(byte_t* src, size_t srcLen) override;
			virtual std::streampos tellp() override;
			virtual abstract_ostream& seekp(std::streampos pos) override;
			virtual abstract_ostream& seekp(std::streamoff off, std::ios_base::seekdir way) override;
			virtual gutils::seek_mode_e get_seek_modep() const override { return gutils::seek_mode_e::forward_seekable; }

			void discard() { assert(!locked()); buffer.clear(); }
			size_t flush_to(tcp::socket& socket_);
			void async_flush_to(tcp::socket& socket_, boost::function<void(const boost::system::error_code&, size_t)> h);
			bool locked() const { return locked_; }
		private:
			constexpr void lock() { locked_ = true; }
			constexpr void unlock() { locked_ = false; }

			buffer_t buffer;
			bool locked_ = false;
		};
	}
}