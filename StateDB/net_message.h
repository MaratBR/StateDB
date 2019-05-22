#pragma once
#include "stdafx.h"

namespace statedb {
	namespace net {
		struct message_out_t
		{
#			define MESSAGE_OUT_ERR 0xee
#			define MESSAGE_OUT_
#			define MESSAGE_OUT_OK 0
#			define MESSAGE_OUT 
			byte_t type;
		};
	}
}