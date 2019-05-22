#pragma once
#include "stdafx.h"

namespace statedb {
	namespace net {
		class client_controller
		{
			virtual bool has_message() = 0;
			virtual ULONG get_identity() = 0;
			virtual bool is_remote() = 0;
		};
	}
}