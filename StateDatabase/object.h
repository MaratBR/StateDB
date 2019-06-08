#pragma once
#include "pch.h"
#include "dtypes.h"

_BEGIN_STATEDB

class invalid_dtype : public std::exception
{
public:
	invalid_dtype(dtypes::dtype_t dt) : 
		std::exception("Invalid Data Type"), invalid_dtype_(dt)
	{
	}

	dtypes::dtype_t dtype() const { return invalid_dtype_; }
private:
	const dtypes::dtype_t invalid_dtype_;
};

class db_object
{
public:
	db_object(std::string key);
	db_object(dtypes::dtype_t dtype, std::string key);

	void set_type(dtypes::dtype_t dtype);
	void clear();

	dtypes::dtype_t get_type() const;

	inline bool is_numeric() const { return get_implementor().is_numeric(); }
	inline bool is_dynamic() const { return get_implementor().is_dynamic(); }

	inline dtypes::impl::implementor_base& get_implementor() const
	{
		return dtypes::get_implementor(get_type()).value();
	}

	size_t get_size();
	size_t write_to(void* to, size_t bufSize);
	size_t load_from(void* from, size_t bufSize);
	std::string& get_key();
private:
	dtypes::data_storage_type ds;
	dtypes::dtype_t dtype;
	std::string key;
};

_END_STATEDB