#pragma once
#include "stdafx.h"


namespace statedb {
	#define HASH_TMPL std::hash

	template <typename T>
	class hasher
	{
	public:
		using hash_backend = HASH_TMPL<T>;
		using return_type = decltype(hash_backend{}(0));
		using value_type = T;

		return_type operator()(T & val)
		{
			return _backend(val);
		}

	private:
		hash_backend _backend;
	};

	template<typename T>
	auto make_hash(T & val)
	{
		return HASH_TMPL<T>()(val);
	}

	inline void hash_combine(std::size_t& seed) { }

	template <typename T, typename... Rest>
	inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		hash_combine(seed, rest...);
	}

#define MAKE_HASHABLE(type, ...) \
    template<> struct ::std::hash<type> {\
        ::std::size_t operator()(const type &_) const {\
            ::std::size_t ret = 0;\
            ::statedb::hash_combine(ret, __VA_ARGS__);\
            return ret;\
        }\
    };
}