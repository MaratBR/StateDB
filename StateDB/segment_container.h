#pragma once
#include "stdafx.h"
#include "utils.h"

namespace statedb {

	struct segmenent_preamble
	{
		uint32_t block_size;
		uint16_t blocks_count;
		bool safe = false;
		size_t hash = 0;
	};

	template<typename T>
	class segment_container : public utils::stream_rw<segment_container<T>>
	{
	public:
		segment_container(T& val, size_t blockSize) : segment_container(val)
		{
			preamble.block_size = blockSize;
			adjust_blocks_count();
		}

		segment_container(T& val) : m_val(val) {}

		void write_to(std::ostream& o) override
		{
			adjust_blocks_count();
			utils::write_object(preamble, o);
			auto startPos = o.tellp();
			utils::write_object(m_val, o);
			auto endPos = o.tellp();
			auto size = endPos - startPos;
			skip_bytes_o(startPos + (std::streampos)(preamble.blocks_count * preamble.block_size) - endPos, o);
		}

		void read_from(std::istream& i) override
		{
			utils::read_object(&preamble, i);
			auto startPos = i.tellg();
			utils::read_object(&m_val, i);
			auto endPos = i.tellg();
			auto size = endPos - startPos;
			blocks_count = size / preamble.block_size + (size % preamble.block_size == 0 ? 0u : 1u);

			skip_bytes_i(startPos + (std::streampos)(blocks_count * preamble.block_size) - endPos, i);
		}

		virtual size_t get_size() const override
		{
			return preamble.blocks_count * preamble.block_size;
		}

	private:
		void adjust_blocks_count()
		{
			size_t size = utils::get_object_size(m_val);
			preamble.blocks_count = size / preamble.block_size + (size % preamble.block_size == 0 ? 0u : 1u);
		}

		void skip_bytes_i(std::streampos count, std::istream& i) { i.seekg(count); }

		void skip_bytes_o(std::streampos count, std::ostream& o)
		{
			void* data = malloc(count);
			memset(data, EMPTY_SPACE_BYTE, count);
			o.write((char*)data, count);
		}

		segmenent_preamble preamble;
		size_t blocks_count = 0;
		T& m_val;
	};
}