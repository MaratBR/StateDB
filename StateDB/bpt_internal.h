#pragma once
#include "stdafx.h"
#include "leaf.h"
#include "bpt_base.h"

namespace statedb {
	namespace bpt {
		template<typename TKey, typename TVal, size_t ORDER>
		class bpt_internal : public bpt_base<TKey, TVal, ORDER>
		{
		public:
			using list_node_type = list_node<TKey, bpt_base<TKey, TVal, ORDER>*>;
			static const size_t max_size = ORDER - 1;

			bpt_internal(bpt_base<TKey, TVal, ORDER>* l)
			{
				m_size = 1;
				m_lesser = new leaf<TKey, TVal, ORDER>();
				m_root = new list_node_type(l->primary_key(), l);
			}

			TVal* get_val_ptr(TKey key) override
			{
				return get_child_ge(key)->get_val_ptr(key);
			}

			bool is_leaf() const noexcept override { return false; }
			TKey primary_key() const override { return m_root->m_pair.key; }

			// Sets value and if required splits internal node and returns new root.
			// Otherwise returns nullptr
			bpt_internal* set(TKey key, TVal val)
			{
				bpt_base<TKey, TVal, ORDER>* child = nullptr;

				child = get_child_ge(key);

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
					bpt_internal* iN = dynamic_cast<bpt_internal*>(child);
					bpt_internal* newInternal = iN->set(key, val);
					if (newInternal != nullptr)
					{
						insert_internal(newInternal);
					}
				}


				if (is_full())
				{
					bpt_internal* newInternal = split();
					return newInternal;
				}
				return nullptr;
			}

			bool delete_key(TKey key)
			{
				auto * child = get_child_ge(key);

				if (child->delete_key(key))
				{
					// Delete was successfull
					if (child->size() == 0)
					{
						// Child is empty and must be deleted
						// But not m_lesser
						if (m_lesser == child)
							return false;

						delete_child(key);
					}
					return true;
				}
				return false;
			}

			size_t size() const noexcept override
			{
				return m_size;
			}

			bool is_full() const noexcept override
			{
				return m_size > max_size;
			}

#if DEBUG
			void debug_print(size_t depth = 0) override
			{
				print_depth(depth);
				std::cout << "<internal size=" << size() << ">" << std::endl;
				print_depth(depth + 1);
				std::cout << "<m_lesser>" << std::endl;
				m_lesser->debug_print(depth + 2);
				print_depth(depth + 1);
				std::cout << "</m_lesser>" << std::endl;

				int i = 0;
				auto* n = m_root;

				while (n != nullptr)
				{
					print_depth(depth + 1);
					std::cout << "<node" << i << " key=" << n->m_pair.key << ">" << std::endl;
					n->m_pair.val->debug_print(depth + 2);
					print_depth(depth + 1);
					std::cout << "</node" << i << ">" << std::endl;
					i++;
					n = n->m_next;
				}

				print_depth(depth);
				std::cout << "</internal>" << std::endl;
			}
#endif

		private:
			bpt_internal() {} // Empty constructor (thx captain obvious thank you very much)

			// Deletes list node with given key from inner list. Does not work with m_lesser.
			// `key` must match the key of child exactly
			void delete_child(TKey key)
			{
				auto* lN = m_root;
				if (lN->m_pair.key == key)
				{
					// Delete first node
					m_root = lN->m_next;
					delete lN;
				}
				else
				{
					while (lN->m_next != nullptr)
					{
						if (lN->m_next->m_pair.key == key)
						{
							auto* n = lN->m_next;
							lN->m_next = n->m_next;
							delete n;
						}
					}
				}
			}

			// Inserts new leaf's pointer into the internal node
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
					auto * node = get_child_ln_ge(l->primary_key());
					auto * n = new list_node_type(l->primary_key(), l);
					list_node_type::insert_after(n, node); // Insert new node `n` after `node`

				}
				m_size++;
			}

			// Inserts new internal node's pointer into the internal node
			void insert_internal(bpt_internal* iN)
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
					list_node_type* node = get_child_ln_ge(iN->primary_key());
					auto * n = new list_node_type(iN->primary_key(), iN->m_root->m_pair.val);
					list_node_type::insert_after(n, node);
				}
				m_size++;
			}

			// Returns list's node, that contains pointer to the child
			// associated with given key
			// IGNORED m_lesser. BE CAREFUL
			//
			// ln stand for "list node" which means it actually returns not a child itself
			// but linked list's node that contains it
			list_node_type* get_child_ln_ge(TKey key)
			{
				auto * node = m_root;
				while (node->m_next != nullptr)
				{
					if (key < node->m_next->m_pair.key)
					{
						break;
					}
					node = node->m_next;
				}
				return node;
			}

			// Returns actual child of the node, unlike get_child_ln_ge that just returns "list node"
			bpt_base<TKey, TVal, ORDER>* get_child_ge(TKey key)
			{
				if (key < primary_key())
				{
					return m_lesser;
				}
				return get_child_ln_ge(key)->m_pair.val;
			}

			// Pointer to the child node containing all key-value pairs where key lesser than root's first key
			// Must be set in constructor. The only child that can be empty.
			bpt_base<TKey, TVal, ORDER>* m_lesser; // REQUIRED

			// Split internal node into two
			bpt_internal* split()
			{
				// Create new root
				bpt_internal* newRoot = new bpt_internal();
				newRoot->m_size = 1;
				newRoot->m_lesser = this;

				// Create right node
				bpt_internal* newInternal = new bpt_internal();

				// Init m_lesser and other
				newInternal->m_lesser = new leaf<TKey, TVal, ORDER>();

				// Move middle node
				size_t middle = m_size / 2; // index of middle node
				auto * middleNode = m_root->skip(middle);

				newInternal->m_root = middleNode->m_next;

				middleNode->m_next = nullptr;
				middleNode->m_pair.val = newInternal; // change node's pointer
				newRoot->m_root = middleNode; // move it

				newInternal->m_size = m_size - middle - 1;
				m_size = middle;

				return newRoot;
			}

			list_node_type* m_root; // REQUIRED
			size_t m_size = 0;
		};
	}
}
