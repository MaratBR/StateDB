#pragma once
#include "pch.h"

_BEGIN_STATEDB_BPT

template<typename TKey, typename TVal>
struct kv_pair
{
public:
	kv_pair(TKey k, TVal v) : val(v), key(k) {}
	TKey key;
	TVal val;
};

template<typename TKey, typename TVal>
struct list_node
{
	using pair_type = kv_pair<TKey, TVal>;
			
	list_node(TKey k, TVal v) : m_pair(k, v) {}

	pair_type m_pair;
	list_node* m_next = nullptr;

	list_node* skip(size_t count) 
	{
		list_node* node = this;

		while (count > 0 && node != nullptr)
		{
			node = node->m_next;
			count--;
		}

		return node;
	}

	// Inserts `node` after `afterThis`
	static void insert_after(list_node* node, list_node* afterThis)
	{
		node->m_next = afterThis->m_next;
		afterThis->m_next = node;
	}
};

template<typename TKey, typename TVal>
class leaf_list
{
public:
	using node_type = list_node<TKey, TVal>;

	void set(TKey key, TVal val)
	{
		node_type* node = m_root;
		node_type* newNode = new node_type(key, val);
		if (m_root == nullptr)
		{
			m_root = newNode;
		}
		else if (key < m_root->m_pair.key)
		{
			newNode->m_next = m_root;
			m_root = newNode;
		}
		else
		{
			while (node->m_next != nullptr && node->m_next->m_pair.key <= key)
				node = node->m_next;

			if (node->m_pair.key == key)
			{
				delete newNode;
				node->m_pair.val = val;
				return; // We don't want to increment size since we didn't actually add anything so we just return
			}
			else
			{
				newNode->m_next = node->m_next;
				node->m_next = newNode;
			}
		}
		m_size++;
	}

	bool split(leaf_list<TKey, TVal>* dest)
	{
		size_t half1 = m_size / 2;

		if (half1 == 0)
		{
			return false;
		}

		dest->m_size = m_size - half1;
		node_type* lastNode = m_root->skip(half1 - 1);
		dest->m_root = lastNode->m_next;
		lastNode->m_next = nullptr;
		m_size = half1;
		return true;
	}

	size_t size() const noexcept { return m_size; }

	TVal * get_val_ptr(TKey key)
	{
		node_type* node = m_root;

		while (node != nullptr)
		{
			if (node->m_pair.key == key)
			{
				return &node->m_pair.val;
			}
			node = node->m_next;
		}

		return nullptr;
	}

	bool delete_key(TKey key)
	{
		m_size--;
		node_type* node = m_root;
		if (key == node->m_pair.key)
		{
			m_root = m_root->m_next;
			delete node;
			return true;
		}
		else
		{
			while (node->m_next != nullptr)
			{
				if (node->m_next->m_pair.key == key)
				{
					node_type* nodeTmp = node->m_next;
					node->m_next = nodeTmp->m_next;
					delete nodeTmp;
					return true;
				}
				node = node->m_next;
			}

			m_size++; // Delete wasn't performed
			return false;
		}
	}

	TKey get_root_key() const { return m_root->m_pair.key; }

	void iterate(boost::function<void(TKey&, TVal&)> fn)
	{
		node_type* node = m_root;

		while (node != nullptr)
		{
			fn(node->m_pair.key, node->m_pair.val);
			node = node->m_next;
		}
	}
private:
	node_type* m_root = nullptr;
	size_t m_size;
};

_END_STATEDB_BPT
