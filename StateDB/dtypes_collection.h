#pragma once
#include "stdafx.h"
#include "exceptions.h"
#include "dtype_decl.h"


namespace statedb
{
	using namespace utils;

	class dtypes_collection : public stream_rw<dtypes_collection>
	{
	public:
		void add(dtype_decl decl);

		bool has(dtype_id_t id)
		{
			return m_dtypes.find(id) != m_dtypes.end();
		}

		dtype_decl get(dtype_id_t id)
		{
			if (!has(id))
				throw db_exception("Type with given ID cannot be found");
			return m_dtypes[id];
		}

		size_t size()
		{
			return m_dtypes.size();
		}

		// Унаследовано через stream_rw
		virtual void write_to(abstract_ostream& o) override;
		virtual void read_from(abstract_istream& i) override;
		virtual size_t get_size() const override;
	private:
		std::map<dtype_id_t, dtype_decl> m_dtypes;
	};
}