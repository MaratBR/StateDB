#include "pch.h"
#include "db.h"
#include "segment_container.h"

_BEGIN_STATEDB

#ifdef DEBUG
void db_inner::debug_print()
{
	spdlog::debug(STR(db_inner) " save_db_hash={} db_hash={}", header.save_db_hash, header.db_hash);
	spdlog::debug(STR(db_inner) " info.name={} info.desription={}", header.info.name, header.info.description);
	spdlog::debug(STR(db_inner) " version={}", header.meta.version);
}
#endif

void _STATEDB db_inner::write_to(_STATEDB_UTILS abstract_ostream& o)
{
	write_object(header, o);
}

void _STATEDB db_inner::read_from(_STATEDB_UTILS abstract_istream& i)
{
	read_object(&header, i);
}

size_t (_STATEDB db_inner::get_size)() const
{
	return header.get_size();
}

_END_STATEDB