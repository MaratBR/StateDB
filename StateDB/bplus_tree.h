#pragma once
/*
namespace statedb {
	using bptree_size_t = uint16_t;

	template<bptree_size_t ORDER, typename TKey, typename TVal>
	class bptree
	{
		static_assert(ORDER > 2);
	public:
		using key_type = TKey;
		using value_type = TVal;
		static const bptree_size_t order = ORDER;

		class bucket_base
		{
		public:
			virtual bool is_leaf() const noexcept = 0;


			virtual bucket_base * insert(key_type key, value_type & val) = 0;
			virtual value_type & get(key_type key) = 0;
			virtual bool has(key_type key) noexcept = 0;
			virtual bool delete_key(key_type key) noexcept = 0;

			bptree_size_t size() const noexcept { return m_size; }
			bool is_full() const noexcept { return size() >= order - 1; }
		protected:
			bptree_size_t m_size = 1;
			key_type m_keys[order];

			bptree_size_t ptr_index(key_type key) const noexcept
			{
				bptree_size_t i;
				for (i = 0; i < this->m_size && m_keys[i] < key; i++);
				return i;
			}
		};

		class internal_bucket : public bucket_base
		{
		public:
			virtual bucket_base * insert(key_type key, value_type & val)
			{
				bucket_base * bucket = m_children[ptr_index(key)];

				if (bucket->is_leaf())
				{
					leaf_bucket * newLeaf = dynamic_cast<leaf_bucket*>(bucket->insert(key, val));
					if (newLeaf != nullptr)
					{
						internal_bucket * newInternal = insert_leaf(newLeaf);
						return newInternal;
					}
				}
				else
				{
					internal_bucket * newInternal = dynamic_cast<internal_bucket*>(bucket->insert(key, val));

					if (newInternal != nullptr)
					{
						internal_bucket * newInternal2 = insert_local(newInternal);
						return newInternal2;
					}
					return nullptr;
				}
			}

			virtual value_type & get(key_type key)
			{
				auto index = ptr_index(key);
				return m_children[index]->get(key);
			}

			virtual bool has(key_type key)
			{
				auto index = ptr_index(key);
				return m_children[index]->has(key);
			}

			virtual bool delete_key(key_type key)
			{
				bucket_base * bucket = m_children[ptr_index(key)];

				if (bucket->delete_key(key))
				{

				}
			}

			virtual bool is_leaf() const { return false; }

			static internal_bucket * create_root(bucket_base * left, bucket_base * right)
			{
				auto key = left->m_keys
			}
		protected:
			constexpr bool is_overloaded()
			{
				return this->m_children[order] != nullptr;
			}

			internal_bucket * insert_leaf(leaf_bucket * newLeaf)
			{
				key_type key = newLeaf->primary_key();
				bptree_size_t index = this->ptr_index(key);

				// Since we've allocated more memory for chuldren's pointers and
				// keys, this will succeed even if bucket is full
				for (bptree_size_t i = size(); i < index; i--)
				{
					this->m_keys[i] = this->m_keys[i - 1];
					this->m_children[i + 1] = m_children[i];
				}
				this->m_keys[index] = key;
				this->m_children[index + 1] = dynamic_cast<bucket_base*>(newLeaf);

				// Now, if it's full, split it
				if (this->is_full())
				{
					return split();
				}
				// Finally, if it wasn't splitted, increase size counter
				this->m_size++;

				return nullptr;
			}

			internal_bucket * insert_local(internal_bucket * b)
			{
				if (this->is_full())
				{
					return this->split(b);
				}
				else
				{
					auto key = b->primary_key();
					auto index = this->ptr_index(key);

					for (bptree_size_t i = this->size(); i > index; i--)
					{
						this->m_children[i] = this->m_children[i - 1];
					}
					this->m_children[index] = b;
					this->m_size++;

					return nullptr;
				}
			}

			internal_bucket * split()
			{
				bptree_size_t half1 = (size() + (this->is_overloaded() ? 1 : 0)) / 2;
				this->m_size = half1;
				inter
			}

			bucket_base * m_children[order + 1];
		};

		class leaf_bucket final : public bptree::bucket_base
		{
		public:
			// Returns true for obvious reason
			bool is_leaf() const noexcept override { return true; }

			key_type primary_key()  const noexcept override { return this->m_keys[0]; }

			// Returns reference to object stored in the leaf
			value_type & get(key_type key) override
			{
				bptree_size_t i;
				for (i = 0; i < this->m_size; i++)
				{
					if (m_data[i]->first == key)
						return m_data[i]->second;
				}
				throw new std::exception("Key not found");
			}

			// Returns true if key is present in the leaf bucket
			bool has(key_type key) noexcept override
			{
				for (bptree_size_t i = 0; i < this->m_size; i++)
				{
					if (m_data[i]->first == key)
						return true;
				}
				return false;
			}

			// Deletes key from the leaf. 
			// Returns true if key was deleted, otherwise returns false.
			// WIP
			bool delete_key(key_type key) noexcept override
			{

			}

			// Inserts new value into the leaf.
			// If leaf is full, splits it and returns new-born leaf.
			bucket_base * insert(key_type key, value_type & val) override
			{
				return _insert(key, val);
			}
		private:
			leaf_bucket * _insert(key_type key, value_type & val)
			{
				auto index = this->ptr_index(key);
				if (this->is_full())
				{
					leaf_bucket newLeaf = split();
					if (newLeaf->primary_key() <= key)
						newLeaf->insert(key, val);
					else
						insert(key, val);
					return newLeaf;
				}
				else
				{
					for (bptree_size_t i = this->size(); i > index; i--)
					{
						m_data[i] = m_data[i - 1];
					}
					m_data[index] = val;
					this->m_size++;
					return nullptr;
				}
			}

			leaf_bucket * split()
			{
				bptree_size_t half = this->size() / 2;

				leaf_bucket * newLeaf = new leaf_bucket();
				newLeaf->m_size = this->size() - half;
				std::memcpy(newLeaf->m_data, m_data + half + 1, newLeaf->size());
				this->m_size = half;

				return newLeaf;
			}

			value_type m_data[order - 1];

			friend class internal_bucket;
		};

	};
}
*/