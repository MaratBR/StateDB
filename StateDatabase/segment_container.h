#pragma once
#include "pch.h"
#include "utils.h"

_BEGIN_STATEDB

struct segmenent_preamble
{
	uint32_t block_size;
	uint16_t blocks_count;
	bool safe = false;
	size_t hash = 0;
};

template<typename T>
class segment_container : public _STATEDB_UTILS  stream_rw<segment_container<T>>
{
public:
	segment_container(T& val, uint32_t blockSize) : segment_container(val)
	{
		preamble.block_size = blockSize;
		adjust_blocks_count();
	}

	segment_container(T& val) : m_val(val) {}

	void write_to(_STATEDB_UTILS abstract_ostream& o) override
	{
		adjust_blocks_count();
		_STATEDB_UTILS write_object(preamble, o);
		std::streampos startPos = o.tellp();
		_STATEDB_UTILS write_object(m_val, o);
		std::streampos endPos = o.tellp();
		skip_bytes_o(startPos + (std::streampos)(preamble.blocks_count * preamble.block_size) - endPos, o);
	}

	void read_from(_STATEDB_UTILS abstract_istream& i) override
	{
		_STATEDB_UTILS read_object(&preamble, i);
		auto startPos = i.tellg();
		_STATEDB_UTILS read_object(&m_val, i);
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
		size_t size = _STATEDB_UTILS get_object_size(m_val);
		preamble.blocks_count = size / preamble.block_size + (size % preamble.block_size == 0 ? 0u : 1u);
	}

	void skip_bytes_i(std::streampos count, _STATEDB_UTILS abstract_istream& i) { assert(i.get_seek_modeg() != _STATEDB_UTILS seek_mode_e::non_seekable); i.seekg(count); }

	void skip_bytes_o(std::streampos count, _STATEDB_UTILS abstract_ostream& o)
	{
		void* data = malloc(count);
		memset(data, STATEDB_EMPTY_SPACE_BYTE, count);
		o.write((char*)data, count);
	}

	segmenent_preamble preamble;
	size_t blocks_count = 0;
	T& m_val;
};

_END_STATEDB
