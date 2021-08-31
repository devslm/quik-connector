//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "ClassInfoMapper.h"

bool toClassInfoDto(lua_State *luaState, ClassInfoDto *classInfo) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for class info data! Current stack value type is: <<{}>> but required table!", luaGetType(luaState, -1));

        return false;
    }
    bool isSuccess = true;

    if (!luaGetTableStringField(luaState, "firmid", &classInfo->firmId)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "name", &classInfo->name)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "code", &classInfo->code)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "npars", &classInfo->npars)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "nsecs", &classInfo->nsecs)) {
        isSuccess = false;
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return isSuccess;
}

string toClassInfoJson(Option<ClassInfoDto> *classInfoOption) {
    if (classInfoOption->isEmpty()) {
        return "{}";
    }
    json jsonObject;
    ClassInfoDto classInfo = classInfoOption->get();

    jsonObject["firmId"] = classInfo.firmId;
    jsonObject["name"] = classInfo.name;
    jsonObject["code"] = classInfo.code;
    jsonObject["npars"] = classInfo.npars;
    jsonObject["nsecs"] = classInfo.nsecs;

    return jsonObject.dump();
}
