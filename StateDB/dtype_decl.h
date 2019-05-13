#pragma once
#include "stdafx.h"
#include "flags.h"

using namespace std;

namespace statedb {
	using dtype_id_t = uint16_t;
	using field_t = dtype_id_t;

	struct dtype_decl
	{
		dtype_id_t id;
		flags_t<2> _flags;
		bool is_dynamic;
#		define UNDEFINED_DTYPE_SIZE SIZE_MAX    // data type can have any size
		size_t max_length; // for dynamic types only
		vector<field_t> fields;
	};

	WRITE_OBJECT_SPECIALIZATION(dtype_decl)
	{
		write_object(_Val.id, _Stream);
		write_object(_Val._flags, _Stream);

		if (!_Val.is_dynamic)
		{
			size_t length = _Val.fields.size();
			write_object(length, _Stream);
			for (field_t & col : _Val.fields)
			{
				write_object(col, _Stream);
			}
		}
	}

	READ_OBJECT_SPECIALIZATION(dtype_decl)
	{
		read_object(&_Dest->id, _Stream);
		read_object(&_Dest->_flags, _Stream);

		_Dest->is_dynamic = _Dest->_flags.get<0>();

		if (!_Dest->is_dynamic)
		{
			// Read sequence of types ID's 
			size_t length;
			read_object(&length, _Stream);
			field_t col;
			for (size_t i = 0; i < length; i++)
			{
				read_object(&col, _Stream);
				_Dest->fields.push_back(col);
			}
		}
		else
		{
			// Read max_length
			read_object(&_Dest->max_length, _Stream);
		}
	}
}