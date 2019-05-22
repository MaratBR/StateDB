#pragma once
#include "stdafx.h"

namespace statedb {
	namespace utils {
		class abstract_ostream
		{
		public:
			virtual void write(byte_t* src, size_t srcLen) = 0;
		};

		class abstract_istream
		{
		public:
			virtual void read(byte_t* dest, size_t count) = 0;
		};

		class abstract_stream : public abstract_ostream, public abstract_istream {};

		class istream_wrp : public abstract_istream
		{
		public:
			istream_wrp(std::istream& is);
			virtual void read(byte_t* dest, size_t count) override;
		private:
			std::istream& is;
		};

		class ostream_wrp : public abstract_ostream
		{
		public:
			ostream_wrp(std::ostream& os);
			virtual void write(byte_t* src, size_t srcLen) override;
		private:
			std::ostream& os;
		};
	}
}