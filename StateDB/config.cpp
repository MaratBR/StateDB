#include "stdafx.h"
#include "config.h"

const size_t cmd_handlers_registry::offsets[5] = { meta_handlers_offset, data_handlers_offset, behv_handlers_offset, struct_handlers_offset, sizeof(cmd_handlers_registry) };
