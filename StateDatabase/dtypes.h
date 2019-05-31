#pragma once
#include "pch.h"

_BEGIN_STATEDB

namespace dtypes
{
	enum dtype_e
	{
		NONE	= 0,

		BLOB	= 1,
		STRING	= 2,

		INT8	= 3,
		UINT8	= 4,

		INT16	= 5,
		UINT16	= 6,

		INT32	= 7,
		UINT32	= 8,

		FLOAT	= 9,
		DOUBLE	= 10,

		BIGINT	= 12
	};

	const std::set<dtype_e> dynamic_types = { BLOB, STRING, BIGINT };

	const std::map<dtype_e, size_t> static_sizes = {
		{NONE, 0},
		{INT8, 1},
		{UINT8, 1},

		{INT16, 2},
		{UINT16, 2},

		{INT32, 4},
		{UINT32, 4},
		{FLOAT, sizeof(float)},
		{DOUBLE, sizeof(double)}
	};

	inline bool has_type(dtype_e dt)
	{
		return dynamic_types.find(dt) != dynamic_types.end() || static_sizes.find(dt) != static_sizes.end();
	}

	inline bool is_dynamic(dtype_e dt)
	{
		return dynamic_types.find(dt) != dynamic_types.end();
	}

	inline size_t get_static_size(dtype_e dt)
	{
		auto it = static_sizes.find(dt);
		if (it != static_sizes.end())
		{
			return it->second;
		}
		return 0;
	}
}

_END_STATEDB