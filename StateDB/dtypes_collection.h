#pragma once
#include "stdafx.h"
#include "exceptions.h"
#include "dtype_decl.h"


namespace statedb
{
	class dtypes_collection : public utils::stream_rw<dtypes_collection>
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
	private:
		std::map<dtype_id_t, dtype_decl> m_dtypes;


		// Унаследовано через stream_rw
		virtual void write_to(std::ostream& o) override;
		virtual void read_from(std::istream& i) override;
	};
}