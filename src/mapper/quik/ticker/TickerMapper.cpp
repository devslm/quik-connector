//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "TickerMapper.h"

bool toTickerDto(lua_State *luaState, TickerDto *ticker) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for ticker data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));
        return false;
    }

    if (!luaGetTableStringField(luaState, "code", &ticker->code)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "name", &ticker->name)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "short_name", &ticker->shortName)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_code", &ticker->classCode)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_name", &ticker->className)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "face_value", &ticker->faceValue)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "face_unit", &ticker->faceUnit)) {
        return false;
    }
    ticker->faceUnit = QuikUtils::getCurrency(ticker->faceUnit);

    if (!luaGetTableNumberField(luaState, "scale", &ticker->scale)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "mat_date", &ticker->matDate)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "lot_size", &ticker->lotSize)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "isin_code", &ticker->isinCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "min_price_step", &ticker->minPriceStep)) {
        return false;
    }
    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

json toTickerJson(const list<TickerDto>& tickers) {
    json jsonArray = json::array();

    for (auto& ticker : tickers) {
        Option<TickerDto> tickerOption(ticker);

        jsonArray.push_back(
            toTickerJson(tickerOption)
        );
    }
    return jsonArray;
}

json toTickerJson(Option<TickerDto>& tickerOption) {
    json jsonObject;

    if (tickerOption.isEmpty()) {
        return jsonObject;
    }
    TickerDto ticker = tickerOption.get();
    jsonObject["code"] = ticker.code;
    jsonObject["name"] = ticker.name;
    jsonObject["shortName"] = ticker.shortName;
    jsonObject["classCode"] = ticker.classCode;
    jsonObject["className"] = ticker.className;
    jsonObject["faceValue"] = ticker.faceValue;
    jsonObject["faceUnit"] = ticker.faceUnit;
    jsonObject["scale"] = ticker.scale;
    jsonObject["matDate"] = ticker.matDate;
    jsonObject["lotSize"] = ticker.lotSize;
    jsonObject["isinCode"] = ticker.isinCode;
    jsonObject["minPriceStep"] = ticker.minPriceStep;

    return jsonObject;
}
