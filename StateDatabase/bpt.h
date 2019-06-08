#pragma once
#include "pch.h"
#include "bpt_internal.h"

_BEGIN_STATEDB_BPT

template<typename TKey, typename TVal, size_t ORDER>
class tree
{
public:
	TVal* get(TKey key)
	{
		if (m_root == nullptr)
			return nullptr;
		return m_root->get_val_ptr(key);
	}

	void iterate(boost::function<void(TKey&, TVal&)> fn)
	{
		m_root->iterate(fn);
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
private:
	bpt_internal<TKey, TVal, ORDER>* m_root = nullptr;
};

_END_STATEDB_BPT