#pragma once


namespace statedb {
	struct db_header {
		const char magic[7] = "state0";
	};
}