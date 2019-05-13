#pragma once
#include "dtype_decl.h"


namespace statedb {
	class db_record 
	{
	public:

	private:
		dtype_id_t id;
		byte_t* raw;
		size_t len;
	};
}