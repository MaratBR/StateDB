#include "pch.h"
#include "dtypes.h"

_BEGIN_STATEDB

/// BLOB IMPLEMENTOR

void dtypes::impl::blob_implementor::init(data_storage_type& ds)
{
	ds = nullptr;
}

bool dtypes::impl::blob_implementor::is_numeric() const
{
	return false;
}

bool dtypes::impl::blob_implementor::is_dynamic() const
{
	return true;
}

size_t dtypes::impl::blob_implementor::load_from(void* from, size_t fromLen, data_storage_type& to)
{
	require_at_least(fromLen, sizeof(uint32_t));
	blob_wrapper blob(from);
	uint32_t size = blob.get_size();
	require_at_least(fromLen, sizeof(uint32_t) + size);

	to = new char[sizeof(uint32_t) + size];
	blob.copy_to(to.raw());

	return sizeof(uint32_t) + size;
}

size_t dtypes::impl::blob_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	blob_wrapper blob(from.raw());
	size_t size = blob.get_size();
	require_at_least(bufSize, sizeof(uint32_t) + size);
	blob.copy_to(to);

	return sizeof(uint32_t) + size;
}

/// BLOB WRAPPER UTILITY

dtypes::impl::blob_wrapper::blob_wrapper(void* data)
	: data(data)
{
}

uint32_t dtypes::impl::blob_wrapper::get_size() const
{
	return *reinterpret_cast<uint32_t*>(data);
}

void* dtypes::impl::blob_wrapper::get_data() const
{
	return reinterpret_cast<void*>(
		reinterpret_cast<uint32_t*>(data) + 1);
}

void dtypes::impl::blob_wrapper::copy_to(void* mem)
{
	std::memcpy(mem, data, get_size() + sizeof(uint32_t));
}

/// BIGINT IMPLEMENTOR

void dtypes::impl::bigint_implementor::init(data_storage_type& ds)
{
	ds = new _STATEDB_UTILS bigint();
}

bool dtypes::impl::bigint_implementor::is_numeric() const
{
	return true;
}

bool dtypes::impl::bigint_implementor::is_dynamic() const
{
	return true;
}

size_t dtypes::impl::bigint_implementor::load_from(void* from, size_t fromLen, data_storage_type& to)
{
	_STATEDB_UTILS bigint& bint = *to.cast_as<_STATEDB_UTILS bigint>();
	blob_wrapper blob(from);
	require_at_least(fromLen, sizeof(uint32_t) + blob.get_size());
	std::string s(reinterpret_cast<char*>(blob.get_data()));
	bint.read(s);
	return blob.get_size() + sizeof(uint32_t);
}

size_t dtypes::impl::bigint_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	_STATEDB_UTILS bigint& bint = *from.cast_as<_STATEDB_UTILS bigint>();
	std::string str = bint.to_string();
	require_at_least(bufSize, sizeof(uint32_t) + str.length());

	*reinterpret_cast<uint32_t*>(to) = static_cast<uint32_t>(str.length());

	std::memcpy(reinterpret_cast<uint32_t*>(to) + 1, str.data(), str.length());
	return sizeof(uint16_t) + str.length();
}

/// IMPLEMENTOR FOR NONE TYPE

void dtypes::impl::none_implementor::init(data_storage_type& ds)
{
	ds = nullptr;
}

bool dtypes::impl::none_implementor::is_numeric() const
{
	return false;
}

bool dtypes::impl::none_implementor::is_dynamic() const
{
	return false;
}

size_t dtypes::impl::none_implementor::load_from(void* from, size_t fromLen, data_storage_type& to)
{
	return 0;
}

size_t dtypes::impl::none_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	return 0;
}

namespace dtypes
{
	namespace impl
	{
		namespace implementors
		{
			int8_implementor int8;
			uint8_implementor uint8;

			int16_implementor int16;
			uint16_implementor uint16;

			int32_implementor int32;
			uint32_implementor uint32;

			bigint_implementor bigint;
			blob_implementor blob_string;

			float_implementor float_impl;
			double_implementor double_impl;

			none_implementor none_impl;

			const std::map<dtype_t, implementor_base*> implementors_ =
			{
				{STATEDB_DTYPE_INT8,		&int8},
				{STATEDB_DTYPE_UINT8,		&uint8},
				{STATEDB_DTYPE_INT16,		&int16},
				{STATEDB_DTYPE_UINT16,	&uint16},
				{STATEDB_DTYPE_INT32,		&int32},
				{STATEDB_DTYPE_UINT32,	&uint32},
				{STATEDB_DTYPE_FLOAT,		&float_impl},
				{STATEDB_DTYPE_DOUBLE,	&double_impl},
				{STATEDB_DTYPE_BLOB,		&blob_string},
				{STATEDB_DTYPE_STRING,	&blob_string},
				{STATEDB_DTYPE_NONE,		&none_impl}
			};
		}
	}
	const std::map<dtype_t, impl::implementor_base*>& implementors()
	{
		return impl::implementors::implementors_;
	}
}

_END_STATEDB
