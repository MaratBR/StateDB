#include "pch.h"
#include "dtypes.h"
#include "dtypes_utils.h"

_BEGIN_STATEDB

#pragma region blob_implementor

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
	uint32_t size = blob.get_size() + sizeof(uint32_t);
	require_at_least(fromLen, size);

	to = new char[size];
	blob.copy_to(to.raw());

	return size;
}

size_t dtypes::impl::blob_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	blob_wrapper blob(from.raw());
	size_t size = blob.get_size() + sizeof(uint32_t);
	require_at_least(bufSize, size);
	blob.copy_to(to);

	return size;
}

size_t dtypes::impl::blob_implementor::get_size(data_storage_type& ds)
{
	blob_wrapper blob(ds.raw());
	return blob.get_size() + sizeof(uint32_t);
}

#pragma endregion

#pragma region blob_wrapper utility class

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

#pragma endregion

#pragma region bigint_implementor

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
	require_at_least(fromLen, 2); // One digit and zero byte
	std::pair<char*, size_t> str = _STATEDB_UTILS get_string_mxsz(reinterpret_cast<char*>(from), fromLen);
	

	std::string s(str.first);
	bint.read(s);
	return str.second;
}

size_t dtypes::impl::bigint_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	_STATEDB_UTILS bigint& bint = *from.cast_as<_STATEDB_UTILS bigint>();
	std::string str = bint.to_string();
	size_t sz = str.length() + 1;
	require_at_least(bufSize, sz);
	std::memcpy(to, str.data(), sz);
	return sz;
}

size_t dtypes::impl::bigint_implementor::get_size(data_storage_type& s)
{
	// TODO this might be optimized actually
	return s.cast_as<_STATEDB_UTILS bigint>()->to_string().length() + 1;
}

#pragma endregion

#pragma region none_implementor

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

size_t dtypes::impl::none_implementor::get_size(data_storage_type& ds)
{
	return 0;
}

#pragma endregion

#pragma region string_implementor

void dtypes::impl::string_implementor::init(data_storage_type& ds)
{
	ds = new char(0);
}
bool dtypes::impl::string_implementor::is_numeric() const
{
	return false;
}
bool dtypes::impl::string_implementor::is_dynamic() const
{
	return true;
}
size_t dtypes::impl::string_implementor::load_from(void* from, size_t fromLen, data_storage_type& to)
{
	std::pair<char*, size_t> str = _STATEDB_UTILS get_string_mxsz(reinterpret_cast<char*>(from), fromLen);
	char* newCh = new char[str.second + 1];
	std::memcpy(newCh, str.first, str.second + 1);
	to = newCh;
	return str.second + 1;
}
size_t dtypes::impl::string_implementor::load_to(void* to, size_t bufSize, data_storage_type& from)
{
	size_t strZLen = get_size(from); // Length of the string including zero-byte
	std::memcpy(to, from.cast_as<void>(), strZLen);
	return strZLen;
}
size_t dtypes::impl::string_implementor::get_size(data_storage_type& s)
{
	return strlen(s.cast_as<char>()) + 1;
}

#pragma endregion

#pragma region implementation global variables

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
			string_implementor string_impl;

			bigint_implementor bigint_impl;

			const std::map<dtype_t, implementor_base*> implementors_ =
			{
				{STATEDB_DTYPE_INT8,		&int8},
				{STATEDB_DTYPE_UINT8,		&uint8},
				{STATEDB_DTYPE_INT16,		&int16},
				{STATEDB_DTYPE_UINT16,		&uint16},
				{STATEDB_DTYPE_INT32,		&int32},
				{STATEDB_DTYPE_UINT32,		&uint32},
				{STATEDB_DTYPE_FLOAT,		&float_impl},
				{STATEDB_DTYPE_DOUBLE,		&double_impl},
				{STATEDB_DTYPE_BLOB,		&blob_string},
				{STATEDB_DTYPE_STRING,		&string_impl},
				{STATEDB_DTYPE_NONE,		&none_impl},
				{STATEDB_DTYPE_BIGINT,		&bigint_impl}
			};
		}
		
	}
	const std::map<dtype_t, impl::implementor_base*>& implementors()
	{
		return impl::implementors::implementors_;
	}
}

#pragma endregion

_END_STATEDB
