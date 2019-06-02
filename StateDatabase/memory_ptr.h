#pragma once
#include "pch.h"

_BEGIN_STATEDB_UTILS

class memory_ptr
{
public:

	memory_ptr& operator=(nullptr_t) 
	{
		delete_inner();
		data = nullptr;
		return *this;
	}


	template<typename T>
	inline memory_ptr& operator=(T* ptr)
	{
		delete_inner();
		data = ptr;
		return *this;
	}

	template<typename T>
	inline T* cast_as()
	{
		return reinterpret_cast<T*>(raw());
	}

	void* raw();

	inline void delete_inner()
	{
		delete[] data;
		data = nullptr;
	}
private:
	void* data = nullptr;
};

_END_STATEDB_UTILS