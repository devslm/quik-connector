//
// Created by Sergey on 12.07.2021.
//

#include "QuikServerMapper.h"

bool toQuikServerConnectionStatusDto(lua_State *luaState, QuikConnectionStatusDto *quikConnectionStatus) {
    double isConnected = lua_tonumber(luaState, -1);
    lua_pop(luaState, 1);

    quikConnectionStatus->isConnected = (isConnected > 0.0 ? true : false);

    return true;
}

json toQuikServerConnectionStatusJson(Option<QuikConnectionStatusDto> *quikConnectionStatusOption) {
    json jsonObject;

    if (quikConnectionStatusOption->isEmpty()) {
        return jsonObject;
    }
    QuikConnectionStatusDto quikConnectionStatus = quikConnectionStatusOption->get();

    jsonObject["isConnected"] = quikConnectionStatus.isConnected;

    return jsonObject;
}

bool toQuikUserInfoDto(lua_State *luaState, QuikUserInfoDto *quikUserInfo) {
    bool isSuccess = luaGetString(luaState, &quikUserInfo->name);

    return isSuccess;
}

json toQuikUserInfoJson(Option<QuikUserInfoDto> *quikUserInfoOption) {
    json jsonObject;

    if (quikUserInfoOption->isEmpty()) {
        return jsonObject;
    }
    QuikUserInfoDto quikUserInfo = quikUserInfoOption->get();

    jsonObject["name"] = quikUserInfo.name;

    return jsonObject;
}
