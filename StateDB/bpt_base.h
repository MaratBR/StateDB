#pragma once

namespace statedb {
	namespace bpt {
		template<typename TKey, typename TVal, size_t ORDER>
		class bpt_base
		{
		public:
			virtual bool is_leaf() const = 0;
			virtual TKey primary_key() const = 0;
		};
	}
}