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

string toQuikServerConnectionStatusJson(Option<QuikConnectionStatusDto> *quikConnectionStatusOption) {
    if (quikConnectionStatusOption->isEmpty()) {
        return "{}";
    }
    json jsonObject;
    QuikConnectionStatusDto quikConnectionStatus = quikConnectionStatusOption->get();

    jsonObject["isConnected"] = quikConnectionStatus.isConnected;

    return jsonObject.dump();
}

bool toQuikUserInfoDto(lua_State *luaState, QuikUserInfoDto *quikUserInfo) {
    bool isSuccess = luaGetString(luaState, &quikUserInfo->name);

    return isSuccess;
}

string toQuikUserInfoJson(Option<QuikUserInfoDto> *quikUserInfoOption) {
    if (quikUserInfoOption->isEmpty()) {
        return "{}";
    }
    json jsonObject;
    QuikUserInfoDto quikUserInfo = quikUserInfoOption->get();

    jsonObject["name"] = quikUserInfo.name;

    return jsonObject.dump();
}
