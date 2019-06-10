#pragma once
#include "pch.h"
#include "dynamic_storage.h"
#include "bigint.h"
#include "memory_ptr.h"

_BEGIN_STATEDB

namespace dtypes
{
	using data_storage_type = _STATEDB_UTILS memory_ptr;

	using dtype_t = uint8_t;

#define STATEDB_DTYPE_NONE 0

#define STATEDB_DTYPE_BLOB 1
#define STATEDB_DTYPE_STRING 2

#define STATEDB_DTYPE_INT8 3
#define STATEDB_DTYPE_UINT8 4

#define STATEDB_DTYPE_INT16 5
#define STATEDB_DTYPE_UINT16 6

#define STATEDB_DTYPE_INT32 7
#define STATEDB_DTYPE_UINT32 8

#define STATEDB_DTYPE_FLOAT	9
#define STATEDB_DTYPE_DOUBLE 10

#define STATEDB_DTYPE_BIGINT 11

	class unsufficient_space : public std::exception
	{
	public:
		unsufficient_space() : std::exception("Not enough space for data") {}
	};

	namespace impl
	{
		inline void require_at_least(size_t bytes, size_t required)
		{
			if (bytes < required)
				throw unsufficient_space();
		}

		class implementor_base
		{
		public:
			virtual void init(data_storage_type& ds) = 0;
			virtual bool is_numeric() const = 0;
			virtual bool is_dynamic() const = 0;
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) = 0;
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) = 0;
			virtual size_t get_size(data_storage_type& s) = 0;
		};

		template<typename T, bool IS_NUMERIC = false>
		class static_implementor : public implementor_base
		{
		public:
			virtual void init(data_storage_type& ds) override
			{
				ds = new T();
			}
			virtual bool is_numeric() const override
			{
				return IS_NUMERIC;
			}
			virtual bool is_dynamic() const override
			{
				return false;
			}
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) override
			{
				require_at_least(fromLen, sizeof(T));
				T& v = *static_cast<T*>(from);
				to = new T(v);
				return sizeof(T);
			}
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) override
			{
				require_at_least(bufSize, sizeof(T));
				*static_cast<T*>(to) = *from.cast_as<T>();
				return sizeof(T);
			}
			virtual size_t get_size(data_storage_type&) override
			{
				return sizeof(T);
			}
		};

		using int8_implementor = static_implementor<int8_t, true>;
		using uint8_implementor = static_implementor<uint8_t, true>;
		using int16_implementor = static_implementor<int16_t, true>;
		using uint16_implementor = static_implementor<uint16_t, true>;
		using int32_implementor = static_implementor<int32_t, true>;
		using uint32_implementor = static_implementor<uint32_t, true>;

		using float_implementor = static_implementor<float, true>;
		using double_implementor = static_implementor<double, true>;

		class blob_wrapper
		{
		public:
			blob_wrapper(void* data);
			uint32_t get_size() const;
			void* get_data() const;
			void copy_to(void* mem);
		private:
			void* data;
		};

		class none_implementor : public implementor_base
		{
		public:

			virtual void init(data_storage_type& ds) override;
			virtual bool is_numeric() const override;
			virtual bool is_dynamic() const override;
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) override;
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) override;
			virtual size_t get_size(data_storage_type& ds) override;
		};

		class blob_implementor : public implementor_base
		{
		public:

			virtual void init(data_storage_type& ds) override;
			virtual bool is_numeric() const override;
			virtual bool is_dynamic() const override;
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) override;
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) override;
			virtual size_t get_size(data_storage_type& ds) override;
		};

		class string_implementor : public implementor_base
		{
		public:
			virtual void init(data_storage_type& ds) override;
			virtual bool is_numeric() const override;
			virtual bool is_dynamic() const override;
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) override;
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) override;
			virtual size_t get_size(data_storage_type& s) override;
		};

		class bigint_implementor : public implementor_base 
		{
		public:

			virtual void init(data_storage_type& ds) override;
			virtual bool is_numeric() const override;
			virtual bool is_dynamic() const override;
			virtual size_t load_from(void* from, size_t fromLen, data_storage_type& to) override;
			virtual size_t load_to(void* to, size_t bufSize, data_storage_type& from) override;
			virtual size_t get_size(data_storage_type& s) override;
		};
	}

	const std::map<dtype_t, impl::implementor_base*>& implementors();

	inline bool has_type(dtype_t dt)
	{
		return implementors().find(dt) != implementors().end();
	}

	inline boost::optional<impl::implementor_base&> get_implementor(dtype_t dt)
	{
		auto it = implementors().find(dt);
		return it == implementors().end() ? boost::none : boost::optional<impl::implementor_base&>(*it->second);
	}
}

_END_STATEDB