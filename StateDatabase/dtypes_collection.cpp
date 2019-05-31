#include "pch.h"
#include "dtypes_collection.h"

_BEGIN_STATEDB

void dtypes_collection::add(dtype_decl decl)
{
	if (decl.id < STATEDB_USER_DTYPE_LOW)
	{
		throw db_exception("ID lesser than " STR(STATEDB_USER_DTYPE_LOW) " is not allowed for custom user types");
	}
	m_dtypes[decl.id] = decl;
}

void dtypes_collection::write_to(_STATEDB_UTILS abstract_ostream& o)
{
	size_t size = m_dtypes.size();
	write_object(size, o);

	for (auto& dtype : m_dtypes)
		write_object(dtype, o);
}

void dtypes_collection::read_from(_STATEDB_UTILS abstract_istream& i)
{
	m_dtypes.clear();

	size_t ii = 0;
	read_object(&ii, i);
	dtype_decl dtype;
	while (ii-- > 0)
	{
		read_object(&dtype, i);
		add(dtype);
	}
}

size_t dtypes_collection::get_size() const
{
	size_t itemsSize = 0;

	for (auto& dtype : m_dtypes)
		itemsSize += dtype.second.get_size();

	return sizeof(size_t) + itemsSize;
}

_END_STATEDB