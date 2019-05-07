#pragma once
#include "stdafx.h"

using namespace std;

namespace statedb {
	using dtype_id_t = uint16_t;
	using field_t = dtype_id_t;

	struct dtype_decl
	{
		dtype_id_t id;
		bool is_dynamic;
		vector<field_t> fields;
	};

	WRITE_OBJECT_SPECIALIZATION(dtype_decl)
	{
		write_object(_Val.id, _Stream);
		write_object(_Val.is_dynamic, _Stream);

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
		read_object(&_Dest->is_dynamic, _Stream);

		if (!_Dest->is_dynamic)
		{
			size_t length;
			read_object(&length, _Stream);
			field_t col;
			for (size_t i = 0; i < length; i++)
			{
				read_object(&col, _Stream);
				_Dest->fields.push_back(col);
			}
		}
	}
}