//
// Created by Sergey on 28.06.2021.
//

#include "ClassInfoMapper.h"

bool toClassInfoDto(lua_State *luaState, ClassInfoDto *classInfo) {
    if (!lua_istable(luaState, -1)) {
        logError("Could not get table for class info data! Current stack value type is: <<%s>> but required table!", luaGetType(luaState, -1));

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
