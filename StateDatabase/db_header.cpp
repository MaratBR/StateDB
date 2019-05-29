#include "pch.h"
#include "db_header.h"

_BEGIN_STATEDB

void db_info::write_to(_STATEDB_UTILS abstract_ostream& o)
{
	utils::write_object(name, o);
	utils::write_object(description, o);
}

void db_info::read_from(_STATEDB_UTILS abstract_istream& i)
{
	utils::read_object(&name, i);
	utils::read_object(&description, i);
}

size_t statedb::db_info::get_size() const
{
	return utils::get_object_size(name) + utils::get_object_size(description);
}

void db_header::write_to(_STATEDB_UTILS abstract_ostream& o)
{
	utils::write_object(save_db_hash, o);
	utils::write_object(debug_mode, o);
	utils::write_object(db_hash, o);
	utils::write_object(meta, o);
	utils::write_object(info, o);
}

void db_header::read_from(_STATEDB_UTILS abstract_istream& i)
{
	utils::read_object(&save_db_hash, i);
	utils::read_object(&debug_mode, i);
	utils::read_object(&db_hash, i);
	utils::read_object(&meta, i);
	utils::read_object(&info, i);
}

size_t db_header::get_size() const
{
	return sizeof(save_db_hash) + sizeof(db_hash) + sizeof(meta) + info.get_size();
}

_END_STATEDB