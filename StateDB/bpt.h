#pragma once
#include "stdafx.h"
#include "bpt_internal.h"

namespace statedb {
	namespace bpt {
		template<typename TKey, typename TVal, size_t ORDER>
		class tree
		{
		public:
#if STATEDB_FEATURE_RAM_MONITORING
			using _RAM_MONITORING_FLAG_ = none_t;
#endif

			TVal* get(TKey key)
			{
				if (m_root == nullptr)
					return nullptr;
				return m_root->get_val_ptr(key);
			}

			bool delete_key(TKey key)
			{
				if (m_root != nullptr)
				{
					bool val = m_root->delete_key(key);

					if (val && m_root->size() == 0)
					{
						// Nod is empty now
						delete m_root;
						m_root = nullptr;
					}

					return val;
				}

				return false;
			}

			void set(TKey key, TVal val)
			{
				if (m_root == nullptr)
				{
					m_root = new bpt_internal<TKey, TVal, ORDER>(new leaf<TKey, TVal, ORDER>(key, val));
				}
				else
				{
					auto* newRoot = m_root->set(key, val);
					if (newRoot != nullptr)
						m_root = newRoot;
				}
			}

#if DEBUG
			void print_debug()
			{
				std::cout << "<tree>" << std::endl;
				if (m_root != nullptr)
					m_root->debug_print();
				std::cout << "</tree>" << std::endl;
			}
#endif
		private:
			bpt_internal<TKey, TVal, ORDER>* m_root = nullptr;
		};
	}
}

template<size_t ORDER>
struct order_t
{
	const size_t order = ORDER;
};


/*
#if STATEDB_FEATURE_RAM_MONITORING

_RAM_MONITORING_NS_BEGIN

template<typename TKey, typename TVal, size_t ORDER>
size_t _RAM_MONITORING_FN_NAME(statedb::bpt::tree<TKey, TVal, ORDER>& t)
{
	return 42;
}

_RAM_MONITORING_NS_END

#endif
*/