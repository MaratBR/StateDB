#include "stdafx.h"
#include "db.h"

void statedb::db_inner::debug_print()

{
	spdlog::debug(STR(db_inner) " save_db_hash={} db_hash={}", header.save_db_hash, header.db_hash);
	spdlog::debug(STR(db_inner) " info.name={} info.desription={}", header.info.name, header.info.description);
	spdlog::debug(STR(db_inner) " version={}", header.meta.version);
	spdlog::debug(STR(db_inner) " dtypes(size={})", dtypes.size());
}

void statedb::db_inner::write_to(std::ostream& o)
{
	utils::write_object(header, o);
	utils::write_object(dtypes, o);
}

void statedb::db_inner::read_from(std::istream& i)
{
	utils::read_object(&header, i);
	utils::read_object(&dtypes, i);
}

size_t statedb::db_inner::get_size() const
{
	return header.get_size() + dtypes.get_size();
}
