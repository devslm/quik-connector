//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include "ParamMapper.h"

static ParamType getParamTypeById(const string& paramType);

bool toParamDto(lua_State *luaState, ParamDto *param) {
    if (!lua_istable(luaState, -1)) {
        LOGGER->error("Could not get table for param ex data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        return false;
    }
    string paramType;

    if (!luaGetTableStringField(luaState, "param_type", &paramType)) {
        return false;
    }
    param->paramType = getParamTypeById(paramType);

    if (!luaGetTableStringField(luaState, "param_value", &param->paramValue)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "param_image", &param->paramImage)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "result", &param->result)) {
        return false;
    }
    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

static ParamType getParamTypeById(const string& paramType) {
    int typeNumber = stoi(paramType);

    switch (typeNumber) {
        case 1:
            return PARAM_DOUBLE_TYPE;
        case 2:
            return PARAM_LONG_TYPE;
        case 3:
            return PARAM_CHAR_TYPE;
        case 4:
            return PARAM_ENUM_TYPE;
        case 5:
            return PARAM_TIME_TYPE;
        case 6:
            return PARAM_DATE_TYPE;
        default:
            return PARAM_UNKNOWN_TYPE;
    }
}

json toParamJson(Option<ParamDto>& paramOption) {
    json jsonObject;

    if (paramOption.isEmpty()) {
        return jsonObject;
    }
    ParamDto param = paramOption.get();
    jsonObject["paramType"] = param.paramType;
    jsonObject["paramValue"] = param.paramValue;
    jsonObject["paramImage"] = param.paramImage;
    jsonObject["result"] = param.result;

    return jsonObject;
}
