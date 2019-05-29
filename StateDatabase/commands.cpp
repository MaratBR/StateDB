#include "pch.h"
#include "commands.h"

const size_t _STATEDB commands::cmd_handlers_registry::offsets[6] = { meta_handlers_offset, data_handlers_offset, behv_handlers_offset, struct_handlers_offset, sizeof(cmd_handlers_registry) };
