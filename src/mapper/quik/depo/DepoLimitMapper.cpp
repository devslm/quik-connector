//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "DepoLimitMapper.h"

bool toDepoLimitDto(lua_State *luaState, DepoLimitDto* depoLimit) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for depo limit data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        return false;
    }

    if (!luaGetTableStringField(luaState, "sec_code", &depoLimit->ticker)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "trdaccid", &depoLimit->trdAccId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "firmid", &depoLimit->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "client_code", &depoLimit->clientCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "openbal", &depoLimit->openBal)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "openlimit", &depoLimit->openLimit)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "currentbal", &depoLimit->currentBal)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "currentlimit", &depoLimit->currentLimit)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "locked_sell", &depoLimit->lockedSell)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "locked_buy", &depoLimit->lockedBuy)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "locked_buy_value", &depoLimit->lockedBuyValue)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "locked_sell_value", &depoLimit->lockedSellValue)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "awg_position_price", &depoLimit->awgPositionPrice)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "limit_kind", &depoLimit->limitKind)) {
        return false;
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

json toDepoLimitJson(Option<DepoLimitDto>& depoLimitOption) {
    json jsonObject;

    if (depoLimitOption.isEmpty()) {
        return jsonObject;
    }
    DepoLimitDto depoLimit = depoLimitOption.get();

    jsonObject["ticker"] = depoLimit.ticker;
    jsonObject["trdAccId"] = depoLimit.trdAccId;
    jsonObject["firmId"] = depoLimit.firmId;
    jsonObject["clientCode"] = depoLimit.clientCode;
    jsonObject["openBal"] = depoLimit.openBal;
    jsonObject["openLimit"] = depoLimit.openLimit;
    jsonObject["currentBal"] = depoLimit.currentBal;
    jsonObject["currentLimit"] = depoLimit.currentLimit;
    jsonObject["lockedSell"] = depoLimit.lockedSell;
    jsonObject["lockedBuy"] = depoLimit.lockedBuy;
    jsonObject["lockedBuyValue"] = depoLimit.lockedBuyValue;
    jsonObject["lockedSellValue"] = depoLimit.lockedSellValue;
    jsonObject["awgPositionPrice"] = depoLimit.awgPositionPrice;
    jsonObject["limitKind"] = depoLimit.limitKind;

    return jsonObject;
}
