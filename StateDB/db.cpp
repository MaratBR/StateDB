#include "stdafx.h"
#include "db.h"
#include "segment_container.h"

using namespace statedb;
using namespace statedb::utils;

void db_inner::debug_print()

{
	spdlog::debug(STR(db_inner) " save_db_hash={} db_hash={}", header.save_db_hash, header.db_hash);
	spdlog::debug(STR(db_inner) " info.name={} info.desription={}", header.info.name, header.info.description);
	spdlog::debug(STR(db_inner) " version={}", header.meta.version);
	spdlog::debug(STR(db_inner) " dtypes(size={})", dtypes.size());
}

void db_inner::write_to(abstract_ostream& o)
{
	write_object(header, o);
	INIT_CONTAINER(container, dtypes);
	write_object(container, o);
}

void db_inner::read_from(abstract_istream& i)
{
	read_object(&header, i);
	INIT_CONTAINER(container, dtypes);
	read_object(&container, i);
}

size_t db_inner::get_size() const
{
	return header.get_size() + dtypes.get_size();
}
