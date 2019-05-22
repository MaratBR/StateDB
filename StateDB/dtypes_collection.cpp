#include "stdafx.h"
#include "dtypes_collection.h"

void statedb::dtypes_collection::add(dtype_decl decl)
{
	if (decl.id < USER_DTYPE_LOW)
	{
		throw db_exception("ID lesser than " STR(USER_DTYPE_LOW) " is not allowed for custom user types");
	}
	m_dtypes[decl.id] = decl;
}

void statedb::dtypes_collection::write_to(std::ostream& o)
{
	size_t size = m_dtypes.size();
	utils::write_object(size, o);

	for (auto& dtype : m_dtypes)
		write_object(dtype, o);
}

void statedb::dtypes_collection::read_from(std::istream& i)
{
	m_dtypes.clear();

	size_t ii = 0;
	utils::read_object(&ii, i);
	dtype_decl dtype;
	while (ii-- > 0)
	{
		utils::read_object(&dtype, i);
		add(dtype);
	}
}

size_t statedb::dtypes_collection::get_size() const
{
	size_t itemsSize = 0;

	for (auto& dtype : m_dtypes)
		itemsSize += dtype.second.get_size();

	return sizeof(size_t) + itemsSize;
}
