//
// Created by Sergey on 12.07.2021.
//

#ifndef QUIK_CONNECTOR_QUIKSERVERMAPPER_H
#define QUIK_CONNECTOR_QUIKSERVERMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../../dto/quik/QuikConnectionStatusDto.h"
#include "../../dto/option/Option.h"
#include "../../dto/quik/QuikUserInfoDto.h"

using namespace nlohmann;
using namespace std;

bool toQuikServerConnectionStatusDto(lua_State *luaState, QuikConnectionStatusDto *quikConnectionStatus);

bool toQuikUserInfoDto(lua_State *luaState, QuikUserInfoDto *quikUserInfo);

json toQuikServerConnectionStatusJson(Option<QuikConnectionStatusDto> *quikConnectionStatusOption);

json toQuikUserInfoJson(Option<QuikUserInfoDto> *quikUserInfoOption);

#endif //QUIK_CONNECTOR_QUIKSERVERMAPPER_H
