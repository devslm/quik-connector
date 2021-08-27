//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DATEMAPPER_H
#define QUIK_CONNECTOR_DATEMAPPER_H

#include <cstdint>
#include "../../../service/lua/Lua.h"

bool toDateMillis(lua_State *L, const char *fieldName, uint64_t *dateTime);

bool toPlainDateMillis(lua_State *L, uint64_t *dateTime);

#endif //QUIK_CONNECTOR_DATEMAPPER_H
