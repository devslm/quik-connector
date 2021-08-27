//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DEPOMAPPER_H
#define QUIK_CONNECTOR_DEPOMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/limit/DepoLimitDto.h"

using namespace nlohmann;
using namespace std;

bool toDepoLimitDto(lua_State *luaState, DepoLimitDto* depoLimit);

json toDepoLimitJson(Option<DepoLimitDto>& depoLimitOption);

#endif //QUIK_CONNECTOR_DEPOMAPPER_H
