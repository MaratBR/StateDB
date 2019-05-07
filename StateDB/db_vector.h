#pragma once
#include "stdafx.h"

namespace statedb {
	namespace utils {
		template<typename T>
		void write_vector(std::vector<T> & vec, std::ostream & s)
		{
			size_t size = vec.size();
			write_object(size, s);

			for (size_t i = 0; i < size; i++)
			{
				write_object(vec[i], s);
			}
		}


		template<typename T>
		void read_vector(std::vector<T> & vec, std::istream & s)
		{
			size_t size;
			read_object(&size, s);
			vec.reserve(size);
			for (size_t i = 0; i < size; i++)
			{
				read_object(&vec[i], s);
			}
		}
	}
}