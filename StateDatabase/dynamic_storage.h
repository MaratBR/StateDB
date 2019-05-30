#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS


class dynamic_storage
{
public:
	dynamic_storage() {}

	bool has_value() const
	{
		return data.size() != 0;
	}

	template<typename T>
	inline void assert_type()
	{
		assert(has_value());
		const char* name = typeid(T).name();
		const char* current = type_name;
		assert(strcmp(name, current) == 0);
	}

	template<typename T>
	T& get()
	{
		assert_type<T>();
		return *(reinterpret_cast<T*>(data.data()));
	}

	template<typename T>
	void set(T& val)
	{
		assert_type<T>();
	}

	template<typename T>
	void allocate()
	{
		deallocate();
		type_name = typeid(T).name();
		size_ = sizeof(T);
		data.resize(size_);
	}

	template<typename T, typename ...TArgs>
	void construct(TArgs... args)
	{
		assert_type<T>();
		std::allocator<T> allocator_;
		allocator_.construct(data.data(), args...);
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