#pragma once
#include "pch.h"

_BEGIN_STATEDB
	
#define STATEDB_HASH_IMPL std::hash

template <typename T>
class hasher
{
public:
	using hash_backend = STATEDB_HASH_IMPL<T>;
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
	return STATEDB_HASH_IMPL<T>()(val);
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

_END_STATEDB

// https://gist.github.com/codebrainz/8ece2a9015a3ed0d260f
namespace std
{
	namespace _cstr_hash_internal
	{
		inline size_t _string_hash(const char* s)
		{
			// http://www.cse.yorku.ca/~oz/hash.html
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	}

	template <>
	struct hash<char*>
	{
		size_t operator()(const char* s) const
		{
			return _cstr_hash_internal::_string_hash(s);
		}
	};

	template <>
	struct hash<const char*>
	{
		size_t operator()(const char* s) const
		{
			return _cstr_hash_internal::_string_hash(s);
		}
	};
}
