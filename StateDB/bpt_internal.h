#pragma once
#include "stdafx.h"
#include "leaf.h"
#include "bpt_base.h"

namespace statedb {
	namespace bpt {
		template<typename TKey, typename TVal, size_t ORDER>
		class internal : public bpt_base<TKey, TVal, ORDER>
		{
		public:
			using list_node_type = list_node<TKey, bpt_base<TKey, TVal, ORDER>*>;

			internal(TKey key, bpt_base<TKey, TVal, ORDER>* l)
			{

			}

			bool is_leaf() const override { return false; }
			TKey primary_key() const override { return m_root->m_pair.key; }

			/// Sets value and if required splits internal node and returns new root.
			/// Otherwise returns nullptr
			internal* set(TKey key, TVal val)
			{
				bpt_base<TKey, TVal, ORDER>* child = nullptr;

				if (key < primary_key())
				{
					child = m_lesser;
				}
				else
				{
					child = get_child_ge(key)->m_pair.val;
				}

				if (child->is_leaf())
				{
					leaf<TKey, TVal, ORDER>* l = dynamic_cast<leaf<TKey, TVal, ORDER>*>(child);
					auto * newLeaf = l->insert(key, val);
					if (newLeaf != nullptr)
					{
						insert_leaf(newLeaf);
					}
				}
				else
				{
					internal* iN = dynamic_cast<internal*>(child);
					internal* newInternal = iN->set(key, val);
					if (newInternal != nullptr)
					{
						insert_internal(newInternal);
					}
				}


				// TODO Split node if required
			}

		private:
			/// Inserts new leaf's pointer into the internal node
			void insert_leaf(leaf<TKey, TVal, ORDER>* l)
			{
				if (l->primary_key() < m_root->m_pair.key)
				{
					// Add to the beginning of the list
					auto * n = new list_node_type(l->primary_key(), l);
					n->m_next = m_root;
					m_root = n;
				}
				else
				{
					// Find appropriate position
					auto * node = get_child_ge(l->primary_key());
					auto * n = new list_node_type(l->primary_key(), l);
					list_node_type::insert_after(n, node); // Insert new node `n` after `node`
					
				}
				m_size++;
			}

			/// Inserts new internal node's pointer into the internal node
			void insert_internal(internal* iN)
			{
				// iN is always a new root, hence it has 2 pointer and 1 key
				if (iN->primary_key() < m_root->m_pair.key)
				{
					// Add to the beginning of the list
					auto * n = new list_node_type(iN->primary_key(), iN);
					n->m_next = m_root;
					m_root = n;
				}
				else
				{
					list_node_type* node = get_child_ge(iN->primary_key());
					auto * n = new list_node_type(iN->primary_key(), iN->m_root->m_pair.val);
					list_node_type::insert_after(n, node);
				}
				m_size++;
			}

			/// Returns list's node, that contains pointer to the child
			/// associated with given key
			list_node_type* get_child_ge(TKey key)
			{
				auto * node = m_root;
				while (node->m_next != nullptr)
				{
					if (key < node->m_next->m_pair.key)
					{
						break;
					}
				}
				return node;
			}

			// Pointer to the child node containing all key-value pairs where key lesser than root's first key
			// Must be set in constructor
			bpt_base<TKey, TVal, ORDER>* m_lesser;

			list_node_type* m_root;
			size_t m_size = 0;
		};
	}
}
