//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_PARAMMAPPER_H
#define QUIK_CONNECTOR_PARAMMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/param/ParamDto.h"

using namespace nlohmann;
using namespace std;

NLOHMANN_JSON_SERIALIZE_ENUM(
    ParamType,
    {
        {PARAM_DOUBLE_TYPE, "DOUBLE"},
        {PARAM_LONG_TYPE, "LONG"},
        {PARAM_CHAR_TYPE, "CHAR"},
        {PARAM_ENUM_TYPE, "ENUM"},
        {PARAM_TIME_TYPE, "TIME"},
        {PARAM_DATE_TYPE, "DATE"},
        {PARAM_UNKNOWN_TYPE, "UNKNOWN"},
    }
);

bool toParamDto(lua_State *luaState, ParamDto *param);

json toParamJson(Option<ParamDto>& paramOption);

#endif //QUIK_CONNECTOR_PARAMMAPPER_H
