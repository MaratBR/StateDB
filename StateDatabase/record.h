#pragma once
#include "pch.h"
#include "dtypes.h"

_BEGIN_STATEDB

namespace db_record_impl
{
	template<typename T>
	struct impl
	{
	};

	template<>
	class impl<int> {};
}

class db_record
{
public:
	dtypes::dtype_e get_type() const;
	void set(void* mem, size_t srcLen);
	void set_type(dtypes::dtype_e type);
	void set(dtypes::dtype_e dtype, void* mem, size_t srcLen);
	size_t size() const;
private:
	dtypes::dtype_e type = dtypes::NONE;
	std::vector<char> data;
};

_END_STATEDB