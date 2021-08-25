//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKSERVERMAPPER_H
#define QUIK_CONNECTOR_QUIKSERVERMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/quik/server/QuikConnectionStatusDto.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/server/QuikUserInfoDto.h"

using namespace nlohmann;
using namespace std;

bool toQuikServerConnectionStatusDto(lua_State *luaState, QuikConnectionStatusDto *quikConnectionStatus);

bool toQuikUserInfoDto(lua_State *luaState, QuikUserInfoDto *quikUserInfo);

json toQuikServerConnectionStatusJson(Option<QuikConnectionStatusDto> *quikConnectionStatusOption);

json toQuikUserInfoJson(Option<QuikUserInfoDto> *quikUserInfoOption);

#endif //QUIK_CONNECTOR_QUIKSERVERMAPPER_H
