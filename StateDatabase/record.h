#pragma once
#include "pch.h"
#include "dtype_decl.h"

_BEGIN_STATEDB

class db_record 
{
public:

private:
	dtype_id_t id;
	byte_t* raw;
	size_t len;
};

_END_STATEDB