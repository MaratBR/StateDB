#pragma once

namespace statedb {
	namespace bpt {
		template<typename TKey, typename TVal, size_t ORDER>
		class bpt_base
		{
		public:
			virtual bool is_leaf() const noexcept = 0;
			virtual TKey primary_key() const = 0;
			virtual size_t size() const noexcept = 0;
			virtual bool is_full() const noexcept = 0;
			virtual bool delete_key(TKey key) = 0;
			virtual TVal* get_val_ptr(TKey key) = 0;
#if DEBUG
			virtual void debug_print(size_t depth = 0) = 0;
#endif
		};
	}
}