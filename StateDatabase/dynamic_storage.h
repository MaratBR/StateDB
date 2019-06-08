#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS


#define STATEDB_DYNAMIC_STORAGE_BINARY "BINARY"

class dynamic_storage
{
public:
	dynamic_storage() {}

	bool has_value() const
	{
		return data.size() != 0;
	}

	inline void assert_has_value()
	{
		assert(has_value());
	}

	inline void assert_type_is(const char* type)
	{
		auto current = get_type();
		assert(current == type);
	}

	template<typename T>
	inline void assert_type()
	{
		assert_has_value();
		assert_type_is(typeid(T).name());
	}

	inline void assert_binary()
	{
		assert_has_value();
		assert_type_is(STATEDB_DYNAMIC_STORAGE_BINARY);
	}

	template<typename T>
	inline bool is() const
	{
		return get_type() == typeid(T).name();
	}

	inline bool is_binary() const
	{
		return get_type() == STATEDB_DYNAMIC_STORAGE_BINARY;
 	}

	template<typename T>
	T& get()
	{
		assert_type<T>();
		return *(reinterpret_cast<T*>(data.data()));
	}

	void* get_binary()
	{
		assert_binary();
		return reinterpret_cast<void*>(data.data());
	}

	template<typename T>
	void set(T& val)
	{
		assert_type<T>();
	}

	void set_binary(void* mem, size_t s)
	{
		assert_binary();
		assert(s <= size_);
		std::memcpy(data.data(), mem, s);
	}

	template<typename T>
	void allocate()
	{
		deallocate();
		type_name = typeid(T).name();
		size_ = sizeof(T);
		data.resize(size_);
	}

	void allocate_binary(size_t s)
	{
		deallocate();
		type_name = STATEDB_DYNAMIC_STORAGE_BINARY;
		size_ = s;
		data.resize(s);
	}

	void allocate_binary(size_t s, void* memory)
	{
		allocate_binary(s);
		std::memcpy(data.data(), memory, s);
	}

	template<typename T, typename ...TArgs>
	inline void allocate_constructed(TArgs... args)
	{
		allocate<T>();
		construct<T>(args...);
	}

	template<typename T>
	inline void copy_from(T& src)
	{
		allocate<T>();
		construct<T>(src);
	}

	template<typename T, typename ...TArgs>
	void construct(TArgs... args)
	{
		assert_type<T>();
		std::allocator<T> allocator_;
		std::allocator_traits<std::allocator<T>>::construct(allocator_, reinterpret_cast<T*>(data.data()), args...);
	}

	void deallocate()
	{
		if (!has_value()) return;
		data.clear();
		type_name = "";
		size_ = 0;
	}

	std::string get_type() const
	{
		return std::string(type_name);
	}

	auto get_asio_buffer()
	{
		return boost::asio::buffer(data.data(), size_);
	}

	template<typename T>
	inline auto allocate_asio_buffer()
	{
		allocate<T>();
		return get_asio_buffer();
	}
private:
	std::vector<char> data;
	size_t size_ = 0;
	const char* type_name = "";
};

_END_STATEDB_UTILS