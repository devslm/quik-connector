//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "TradeAccountMapper.h"

bool toTradeAccountDto(lua_State *luaState, TradeAccountDto* tradeAccount) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for trade accounts data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }
    string classCodes;

    if (!luaGetTableStringField(luaState, "class_codes", &classCodes)) {
        return false;
    }
    tradeAccount->classCodes = stringSplitByDelimeter(classCodes, '|');

    if (!luaGetTableStringField(luaState, "firmid", &tradeAccount->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "trdaccid", &tradeAccount->trdAccId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "description", &tradeAccount->description)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "fullcoveredsell", &tradeAccount->fullCoveredSell)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "main_trdaccid", &tradeAccount->mainTrdAccId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "bankid_t0", &tradeAccount->bankIdT0)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "bankid_tplus", &tradeAccount->bankIdTplus)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "trdacc_type", &tradeAccount->trdAccType)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "depunitid", &tradeAccount->depUnitId)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "status", &tradeAccount->status)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "firmuse", &tradeAccount->firmUse)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "depaccid", &tradeAccount->depAccId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "bank_acc_id", &tradeAccount->bankAccId)) {
        return false;
    }
    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

json toTradeAccountJson(const list<TradeAccountDto>& tradeAccounts) {
    json jsonArray = json::array();

    for (auto& tradeAccount : tradeAccounts) {
        Option<TradeAccountDto> tradeAccountOption(tradeAccount);

        jsonArray.push_back(
            toTradeAccountJson(tradeAccountOption)
        );
    }
    return jsonArray;
}

json toTradeAccountJson(Option<TradeAccountDto>& tradeAccountOption) {
    json jsonObject;

    if (tradeAccountOption.isEmpty()) {
        return jsonObject;
    }
    TradeAccountDto tradeAccount = tradeAccountOption.get();
    jsonObject["classCodes"] = tradeAccount.classCodes;
    jsonObject["firmId"] = tradeAccount.firmId;
    jsonObject["trdAccId"] = tradeAccount.trdAccId;
    jsonObject["description"] = tradeAccount.description;
    jsonObject["fullCoveredSell"] = tradeAccount.fullCoveredSell;
    jsonObject["mainTrdAccId"] = tradeAccount.mainTrdAccId;
    jsonObject["bankIdT0"] = tradeAccount.bankIdT0;
    jsonObject["bankIdTplus"] = tradeAccount.bankIdTplus;
    jsonObject["trdAccType"] = tradeAccount.trdAccType;
    jsonObject["depUnitId"] = tradeAccount.depUnitId;
    jsonObject["status"] = tradeAccount.status;
    jsonObject["firmUse"] = tradeAccount.firmUse;
    jsonObject["depAccId"] = tradeAccount.depAccId;
    jsonObject["bankAccId"] = tradeAccount.bankAccId;

    return jsonObject;
}
