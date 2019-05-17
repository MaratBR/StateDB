#pragma once
#include <istream>
#include <ostream>

namespace statedb {
	namespace utils {
		template<typename TDerived>
		class stream_rw
		{
		public:
			friend std::istream& operator >> (std::istream& is, stream_rw& _this) { read_object(dynamic_cast<TDerived*>(&_this), is); return is; }
			friend std::ostream& operator << (std::ostream& os, stream_rw& _this) { write_object(*dynamic_cast<TDerived*>(&_this), os); return os; }

			virtual void write_to(std::ostream& o) = 0;
			virtual void read_from(std::istream& i) = 0;
			virtual size_t get_size() const = 0;
		};

		template <
			typename T,
			std::enable_if_t<!std::is_base_of<stream_rw<T>, T>::value, int> = 0
		>
		constexpr size_t get_object_size(T& obj)
		{
			return sizeof(T);
		}

		template <
			typename T,
			std::enable_if_t<!std::is_base_of<stream_rw<T>, T>::value, int> = 0
		>
		constexpr size_t get_object_size(T & obj)
		{
			return dynamic_cast<stream_rw<T>*>(&obj)->get_size();
		}

		void write_object_generic(void * ptr, size_t size, std::ostream & o);

		template <
			typename T, 
			std::enable_if_t<!std::is_base_of<stream_rw<T>, T>::value, int> = 0>
		void write_object(T & val, std::ostream & o)
		{
			write_object_generic(reinterpret_cast<void*>(&val), sizeof(T), o);
		}

		void read_object_generic(void * dest, size_t size, std::istream & i);

		template <
			typename T, 
			std::enable_if_t<!std::is_base_of<stream_rw<T>, T>::value, int> = 0>
		void read_object(T * dest, std::istream & i)
		{
			read_object_generic(reinterpret_cast<void*>(dest), sizeof(T), i);
		}

		template <
			typename T, 
			std::enable_if_t<std::is_base_of<stream_rw<T>, T>::value, int> = 0>
		void write_object(T& val, std::ostream& o)
		{
			dynamic_cast<stream_rw<T>*>(&val)->write_to(o);
		}

		template <
			typename T, 
			std::enable_if_t<std::is_base_of<stream_rw<T>, T>::value, int> = 0>
		void read_object(T* dest, std::istream& i)
		{
			dynamic_cast<stream_rw<T>*>(dest)->read_from(i);
		}

#define WRITE_OBJECT_SPECIALIZATION(t) template<> void ::statedb::utils::write_object<t, 0>(t & _Val, std::ostream & _Stream)
#define READ_OBJECT_SPECIALIZATION(t) template<> void ::statedb::utils::read_object<t, 0>(t * _Dest, std::istream & _Stream)
#define WRITE_OBJECT_F(field) _Stream.write(reinterpret_cast<char*>(&_Val.field), sizeof(decltype(_Val.field)))
#define CLASS_RW_FN_FRIEND(T) \
friend void utils::read_object(dtypes_collection*, std::istream&); \
friend void utils::write_object(dtypes_collection&, std::ostream&);
	}
}