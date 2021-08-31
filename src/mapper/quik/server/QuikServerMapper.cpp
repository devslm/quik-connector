//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikServerMapper.h"

bool toQuikServerConnectionStatusDto(lua_State *luaState, QuikConnectionStatusDto *quikConnectionStatus) {
    double isConnected = 0.0;

    if (!luaGetNumber(luaState, &isConnected)) {
        return false;
    }
    quikConnectionStatus->isConnected = (isConnected > 0.0 ? true : false);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

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

    luaPrintStackSize(luaState, (string)__FUNCTION__);

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
