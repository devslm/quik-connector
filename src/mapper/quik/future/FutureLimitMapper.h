//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_FUTURELIMITMAPPER_H
#define QUIK_CONNECTOR_FUTURELIMITMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/limit/FutureLimitDto.h"

using namespace nlohmann;
using namespace std;

bool toFutureLimitDto(lua_State *luaState, FutureLimitDto* futureLimit);

json toFutureLimitJson(Option<FutureLimitDto>& futureLimitOption);

#endif //QUIK_CONNECTOR_FUTURELIMITMAPPER_H
