#pragma once
#include "stdafx.h"
/*
namespace statedb {
	using bplus_degree_t = uint8_t;

	template<typename TKey, typename TVal, bplus_degree_t degree>
	class bplus_tree
	{
		static_assert(degree > 2);
	public:
		static const bplus_degree_t tree_degree = degree;
		static const bplus_degree_t keys_count = tree_degree - 1;
		using value_type = TVal;
		using key_type = TKey;

		class leaf_node
		{
		public:
			using pair_type = std::pair<key_type, value_type>;
			using iteration_function_t = std::function<void(pair_type&)>;

			leaf_node(key_type key, value_type val)
			{
				m_size = 1;
				m_data[0] = new pair_type(key, val);
			}

			bool insert(key_type key, value_type val)
			{
				bplus_degree_t i;
				if (key < get_first_key()) throw new std::exception("Invalid key");
				for (i = 0; i < m_size && key >= m_data[i]->first; i++);

				leaf_node * newNode = nullptr;
				pair_type * lastPair = nullptr;
				if (i == m_size && is_full())
				{
					lastPair = new pair_type(key, val);
				}
				else
				{
					for (bplus_degree_t j = keys_count - 1; j > i; j--)
					{
						m_data[j] = m_data[j - 1];
					}
					m_data[i] = new pair_type(key, val);

					if (is_full())
					{
						lastPair = m_data[bplus_tree::keys_count - 1];
					}
				}

				if (lastPair != nullptr)
				{
					// Split leaf node
					bplus_degree_t half1 = (m_size + 1) / 2 + (m_size + 1) % 2;

					newNode = new leaf_node(m_data + half1, m_size - half1);
					newNode->m_data[newNode->m_size++] = lastPair;
					m_size = half1;
					newNode->m_next = m_next;
					m_next = newNode;
				}
				else m_size++;

				return newNode != nullptr;
			}

			void iterate(iteration_function_t func, bool chained = false)
			{
				for (bplus_degree_t i = 0; i < m_size; i++)
				{
					func(*m_data[i]);
				}

				if (chained && has_next())
				{
					get_next()->iterate(func, true);
				}
			}

			bool is_full() { return m_size == keys_count; }

			key_type get_first_key()
			{
				return m_data[0]->first;
			}

			leaf_node * get_next() const { return m_next; }
			bool has_next() const { return get_next() != nullptr; }
		private:
			leaf_node(pair_type ** src, bplus_degree_t size)
			{
				memcpy(m_data, src, size * sizeof(pair_type*));
				m_size = size;
			}

			leaf_node::pair_type * m_data[static_cast<size_t>(keys_count)];
			bplus_degree_t m_size;
			leaf_node * m_next = nullptr;
		};

		class internal_node
		{
			internal_node(leaf_node & leaf)
				: m_is_leafparent(true), m_size(1)
			{
				m_pointers[0].leaf_node = &leaf;
			}

			internal_node(key_type key, value_type & value)
				: m_is_leafparent(true), m_size(1)
			{
				m_pointers[0].leaf_node = new leaf_node(key, value);
			}

			bool is_full() { return m_size == keys_count; }

			internal_node * insert(key_type key, value_type val)
			{
				ptr_t child = get_child(key);
				if (m_is_leafparent)
				{
					bool leafCreated = child.leaf_node->insert();

					if (leafCreated)
					{
						leaf_node * newLeaf = child.leaf_node->get_next();
						key_type newKey = newLeaf->get_first_key();
					}
				}
				else
				{
					internal_node * newNode = child.internal_node->insert(key, val);
				}

				
			}
		private:
			using ptr_t = union { internal_node * internal_node; leaf_node * leaf_node; };

			internal_node * insert_new_child(leaf_node * leaf)
			{
				// Find index of new key
				bplus_degree_t i;
				key_type key = leaf->get_first_key();
				for (i = 0; i < m_size && key > m_keys[i]; i++);

				struct { key_type key; ptr_t ptr; } lastChild{ 0 };
				if (is_full())
				{
					if (i == m_size)
					{
						lastChild.key = key;
						lastChild.ptr.leaf_node = leaf;
					}
					else
					{
						lastChild.key = m_keys[keys_count - 1];
						lastChild.ptr = m_pointers[tree_degree - 1];
					}
				}


				for (bplus_degree_t j = m_size; j > i; j--)
				{
					m_keys[j] = m_keys[j - 1];
				}
				m_keys[i] = key;

				for (bplus_degree_t j = m_size + 1; j > i + 1; j--)
				{
					m_pointers[j] = m_pointers[j - 1];
				}
				m_pointers[i + 1].leaf_node = leaf;

				if (lastChild.ptr != nullptr)
				{
					bplus_degree_t rootIndex = (m_size + 1) / 2;
					// Split into two
					internal_node * newNode = new internal_node();
					newNode->m_size = m_size - rootIndex - 1;
					std::memcpy(newNode->m_pointers, m_pointers + rootIndex + 1, newNode->m_size * sizeof(ptr_t));

					m_size = rootIndex;

					internal_node * newRoot = new internal_node();
					newRoot->m_size = 1;
					newRoot->m_keys[0] = m_keys[rootIndex];
					newRoot->m_pointers[0].internal_node = this;
					newRoot->m_pointers[0].internal_node = newNode;

					return newRoot;
				}

				return nullptr;
			}

			ptr_t get_child(key_type key)
			{
				bplus_degree_t i;
				for (i = 0; i < m_size && key > m_keys[i]; i++);
				ptr_t p = m_pointers[i];
				return p.leaf_node;
			}

			key_type m_keys[keys_count];
			ptr_t m_pointers[tree_degree];
			bplus_degree_t m_size;
			bool m_is_leafparent;
		};

	private:

	};
}*/