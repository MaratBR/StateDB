#pragma once
#include "pch.h"

#include "list.h"
#include "bpt_base.h"

_BEGIN_STATEDB_BPT

template<typename TKey, typename TVal, size_t ORDER>
class leaf : public bpt_base<TKey, TVal, ORDER>
{
public:
	static const size_t order = ORDER;
	static const size_t max_size = ORDER - 1;
	using key_type = TKey;
	using value_type = TVal;
	using list_type = leaf_list<TKey, TVal>;

	// Empty constructor used for creating empty nodes
	// Remember: empty node will cause a segmentation fault when calling primary_key()
	leaf() {}
			
	leaf(TKey key, TVal val)
	{
		m_list.set(key, val);
	}

	TVal* get_val_ptr(TKey key) override
	{
		return m_list.get_val_ptr(key);
	}

	bool delete_key(TKey key)
	{
		return m_list.delete_key(key);
	}

	leaf<TKey, TVal, ORDER>* insert(key_type key, value_type& val)
	{
		m_list.set(key, val);

		if (size() > max_size)
		{
			return split();
		}
		return nullptr;
	}

	size_t size() const noexcept override { return m_list.size(); }
	bool has_next() const noexcept { return m_next != nullptr; }
	leaf<TKey, TVal, ORDER>* const get_next() const { return m_next; }
	const list_type& inner() const noexcept { return m_list; }
	bool is_leaf() const noexcept override { return true; }
	TKey primary_key() const override { return this->m_list.get_root_key(); }
	bool is_full() const noexcept override { return size() > max_size; }
#if DEBUG
	void debug_print(size_t depth = 0) override
	{
		print_depth(depth);
		std::cout << "<leaf size=" << size() << " has_next=" << (m_next == nullptr ? "no" : "yes") << ">" << std::endl;
		m_list.debug_print(depth + 1);
		print_depth(depth);
		std::cout << "</leaf>" << std::endl;
	}
#endif
			
private:

	leaf<TKey, TVal, ORDER>* split()
	{
		leaf<TKey, TVal, ORDER>* newLeaf = new leaf<TKey, TVal, ORDER>();
		size_t half1 = size() / 2;
				
		if (!m_list.split(&newLeaf->m_list))
		{
			delete newLeaf;
			return nullptr;
		}

		if (has_next())
		{
			newLeaf->m_next = m_next;
		}
		m_next = newLeaf;

		return newLeaf;
	}

	leaf<TKey, TVal, ORDER>* m_next = nullptr;
	list_type m_list;
};

_END_STATEDB_BPT