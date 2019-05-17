#include "stdafx.h"
#include "db_header.h"

void statedb::db_info::write_to(std::ostream& o)
{
	o << name << description;
}

void statedb::db_info::read_from(std::istream& i)
{
	i >> name >> description;
}

size_t statedb::db_info::get_size() const
{
	return name.get_size() + description.get_size();
}

void statedb::db_header::write_to(std::ostream& o)
{
	utils::write_object(save_db_hash, o);
	utils::write_object(db_hash, o);
	utils::write_object(meta, o);
	utils::write_object(info, o);
}

void statedb::db_header::read_from(std::istream& i)
{
	utils::read_object(&save_db_hash, i);
	utils::read_object(&db_hash, i);
	utils::read_object(&meta, i);
	utils::read_object(&info, i);
}

size_t statedb::db_header::get_size() const
{
	return sizeof(save_db_hash) + sizeof(db_hash) + sizeof(meta) + info.get_size();
}
