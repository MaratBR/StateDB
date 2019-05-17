#pragma once
#include "stdafx.h"
#include "utils.h"

namespace statedb {

	struct segmenet_preamble
	{
		size_t block_size;
	};

	template<typename T>
	class segment_container : public utils::stream_rw<segment_container>
	{
	public:
		segment_container(T& val) : m_val(val)
		{
		}

		void write_to(std::ostream& o) override
		{

		}

		void read_from(std::istream& i) override
		{
			utils::read_object(&preamble, i);
			auto startPos = i.tellg();
			utils::read_object(&m_val, i);
			auto endPos = i.tellg();
			auto size = endPos - startPos;
			blocks_count = size / preamble.block_size + (size % preamble.block_size == 0 ? 0u : 1u);

			skip_bytes(startPos + blocks_count * preamble.block_size - endPos);
		}

	private:

		void skip_bytes_i(std::streampos count, std::istream& i) { i.seekg(pos); }

		void skip_bytes_o(std::streampos count, std::istream& o)
		{
			#if (SKIP_UNUSED_BYTES_MODE == SKIP_UNUSED_BYTES_MODE_SEEK)
				i.seekg(pos);
			#else
				#if (SKIP_UNUSED_BYTES_MODE == SKIP_UNUSED_BYTES_MODE_FILL0)
					#define _FILL_BYTE_ 0
				#elif (SKIP_UNUSED_BYTES_MODE == SKIP_UNUSED_BYTES_MODE_FILL1)
					#define _FILL_BYTE_ 0xff
				#endif
			byte_t* data = new byte_t[count];
			memset(data, _FILL_BYTE_, count);
			#undef _FILL_BYTE_
			#endif
		}

		segmenet_preamble preamble;
		size_t blocks_count = 0;
		T& m_val;
	};
}