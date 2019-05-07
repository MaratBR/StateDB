#pragma once

namespace statedb {
	namespace utils {
		template<typename T>
		struct has_iloader {
			template<typename U, U> struct Check;

			template<typename U>
			static std::true_type _test(Check<void(*)(std::istream&, T*), &U::load_from>*) {}

			template<typename U>
			static std::false_type _test(...) {}

			static const bool value = decltype(_test<T>(0))::value;
		};

		template<typename T>
		struct has_oloader {
			template<typename U, U> struct Check;

			template<typename U>
			static std::true_type _test(Check<void(*)(std::ostream&, T&), &U::write_to>*) {}

			template<typename U>
			static std::false_type _test(...) {}

			static const bool value = decltype(_test<T>(0))::value;
		};

		template<typename T>
		struct has_loader {
			static const bool value = has_iloader<T>::value && has_oloader<T>::value;
		};

		void write_object_generic(void * ptr, size_t size, std::ostream & o);

		template <typename T>
		void write_object(T & val, std::ostream & o)
		{
			write_object_generic(reinterpret_cast<void*>(&val), sizeof(T), o);
		}


#define WRITE_OBJECT_SPECIALIZATION(t, ...) template<__VA_ARGS__> void ::statedb::utils::write_object<t>(t & _Val, std::ostream & _Stream)
#define READ_OBJECT_SPECIALIZATION(t, ...) template<__VA_ARGS__> void ::statedb::utils::read_object<t>(t * _Dest, std::istream & _Stream)
#define WRITE_OBJECT_F(field) _Stream.write(reinterpret_cast<char*>(&_Val.field), sizeof(decltype(_Val.field)))

		void read_object_generic(void * dest, size_t size, std::istream & i);

		template<typename T>
		void read_object(T * dest, std::istream & i)
		{
			read_object_generic(reinterpret_cast<void*>(dest), sizeof(T), i);
		}
	}
}