#pragma once
#include "stdafx.h"
#include <hash_map>

namespace statedb {
	template<typename TPointer, typename T>
	struct hash_key_pair
	{
		hasher<T>::return_type hash;
		size_t ptr;
	};

	template<typename TKey, typename TVal>
	class db_hashmap
	{
	public:
		using hash_pair = hash_key_pair<>;
	};
}