//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "CandleMapper.h"

static const int CANDLE_FIRST_INDEX = 1;

static void validateCandleStartIndex(int *candleFirstIndex, int candleLastIndex);

static json toCandleValueJson(const CandleValueDto& candleValue);

bool toCandleDto(QuikSubscriptionDto *candleSubscription, CandleDto* candle, int candleFirstIndex, int candleLastIndex) {
    validateCandleStartIndex(&candleFirstIndex, candleLastIndex);

    candle->classCode = candleSubscription->classCode;
    candle->ticker = candleSubscription->ticker;
    candle->interval = QuikUtils::getIntervalName(candleSubscription->interval);
    candle->candleLastIndex = candleLastIndex;
    bool isSuccess = true;

    lock_guard<recursive_mutex> lockGuard(*candleSubscription->mutexLock);

    int dataSource = luaLoadReference(candleSubscription->luaState, candleSubscription->dataSourceIndex);

    LOGGER->debug("Convert lua candles data to dto with from index: {} and last index: {}",
        candleFirstIndex, candleLastIndex);

    for (int candleCurrentIndex = candleFirstIndex; candleCurrentIndex <= candleLastIndex; ++candleCurrentIndex) {
        CandleValueDto candleValue;

        lua_getfield(candleSubscription->luaState, dataSource, "O");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);

        bool result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            candleValue.open = lua_tonumber(candleSubscription->luaState, -1);

            lua_pop(candleSubscription->luaState, 1);
        } else {
            isSuccess = false;
        }
        lua_getfield(candleSubscription->luaState, dataSource, "C");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            candleValue.close = lua_tonumber(candleSubscription->luaState, -1);

            lua_pop(candleSubscription->luaState, 1);
        } else {
            isSuccess = false;
        }
        lua_getfield(candleSubscription->luaState, dataSource, "H");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            candleValue.high = lua_tonumber(candleSubscription->luaState, -1);

            lua_pop(candleSubscription->luaState, 1);
        } else {
            isSuccess = false;
        }
        lua_getfield(candleSubscription->luaState, dataSource, "L");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            candleValue.low = lua_tonumber(candleSubscription->luaState, -1);

            lua_pop(candleSubscription->luaState, 1);
        } else {
            isSuccess = false;
        }
        lua_getfield(candleSubscription->luaState, dataSource, "V");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            candleValue.volume = lua_tonumber(candleSubscription->luaState, -1);

            lua_pop(candleSubscription->luaState, 1);
        } else {
            isSuccess = false;
        }
        lua_getfield(candleSubscription->luaState, dataSource, "T");
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK == result) {
            if (!toPlainDateMillis(candleSubscription->luaState, &candleValue.date)) {
                isSuccess = false;
            }
        } else {
            isSuccess = false;
        }
        candle->values.push_back(candleValue);
    }
    // Remove datasource structure from stack
    lua_pop(candleSubscription->luaState, 1);

    luaPrintStackSize(candleSubscription->luaState, (string)__FUNCTION__);

    return isSuccess;
}

static void validateCandleStartIndex(int *candleFirstIndex, const int candleLastIndex) {
    if (*candleFirstIndex < CANDLE_FIRST_INDEX) {
        LOGGER->warn("Could not convert candles to dto with the first index: {} because candles list starts from: {} so use: {} as first index",
            *candleFirstIndex, CANDLE_FIRST_INDEX, CANDLE_FIRST_INDEX);

        *candleFirstIndex = CANDLE_FIRST_INDEX;
    }

    if (*candleFirstIndex > candleLastIndex) {
        LOGGER->warn("Could not convert candles to dto with the first index: {} because first index should be less or equal last index: {} so use last index as first index",
            *candleFirstIndex, candleLastIndex, candleLastIndex);

        *candleFirstIndex = candleLastIndex;
    }
}

json toCandleJson(Option<CandleDto>& candleOption) {
    json jsonObject;

    if (candleOption.isEmpty()) {
        return jsonObject;
    }
    const auto candle = candleOption.get();

    jsonObject["classCode"] = candle.classCode;
    jsonObject["ticker"] = candle.ticker;
    jsonObject["interval"] = candle.interval;
    jsonObject["candleLastIndex"] = candle.candleLastIndex;
    jsonObject["values"] = json::array();

    for (const auto& candleValue : candle.values) {
        jsonObject["values"].push_back(
            toCandleValueJson(candleValue)
        );
    }
    return jsonObject;
}

Option<ChangedCandleDto> toChangedCandleDto(Option<CandleDto>& candleOption) {
    ChangedCandleDto changedCandle;

    if (candleOption.isEmpty()) {
        return Option<ChangedCandleDto>();
    }
    CandleDto candle = candleOption.get();
    changedCandle.classCode = candle.classCode;
    changedCandle.ticker = candle.ticker;
    changedCandle.interval = candle.interval;
    changedCandle.currentCandle = candle.values.back();

    candle.values.pop_back();

    if (!candle.values.empty()) {
        changedCandle.previousCandle = candle.values.back();
    }
    return Option<ChangedCandleDto>(changedCandle);
}

json toChangedCandleJson(Option<ChangedCandleDto>& changedCandleOption) {
    json jsonObject;

    if (changedCandleOption.isEmpty()) {
        return jsonObject;
    }
    const auto changedCandle = changedCandleOption.get();

    jsonObject["classCode"] = changedCandle.classCode;
    jsonObject["ticker"] = changedCandle.ticker;
    jsonObject["interval"] = changedCandle.interval;
    jsonObject["previousCandle"] = toCandleValueJson(changedCandle.previousCandle);
    jsonObject["currentCandle"] = toCandleValueJson(changedCandle.currentCandle);

    return jsonObject;
}

static json toCandleValueJson(const CandleValueDto& candleValue) {
    json candleValuesJson;
    candleValuesJson["open"] = candleValue.open;
    candleValuesJson["close"] = candleValue.close;
    candleValuesJson["high"] = candleValue.high;
    candleValuesJson["low"] = candleValue.low;
    candleValuesJson["volume"] = candleValue.volume;
    candleValuesJson["date"] = candleValue.date;

    return candleValuesJson;
}
