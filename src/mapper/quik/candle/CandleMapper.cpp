//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
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

    logger->debug("Convert lua candles data to dto with from index: {} and last index: {}",
        candleFirstIndex, candleLastIndex);

    for (int candleCurrentIndex = candleFirstIndex; candleCurrentIndex <= candleLastIndex; ++candleCurrentIndex) {
        CandleValueDto candleValue;

        if(!luaGetField(candleSubscription->luaState, dataSource, "O")) {
            logger->error("Could not get candle open price with class code: {}, ticker: {} and interval: {} because could not get lua field <<O>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);

        bool result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK != result || !luaGetNumber(candleSubscription->luaState, &candleValue.open)) {
            isSuccess = false;
        }
        if(!luaGetField(candleSubscription->luaState, dataSource, "C")) {
            logger->error("Could not get candle close price with class code: {}, ticker: {} and interval: {} because could not get lua field <<C>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK != result || !luaGetNumber(candleSubscription->luaState, &candleValue.close)) {
            isSuccess = false;
        }

        if(!luaGetField(candleSubscription->luaState, dataSource, "H")) {
            logger->error("Could not get candle high price with class code: {}, ticker: {} and interval: {} because could not get lua field <<H>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK != result || !luaGetNumber(candleSubscription->luaState, &candleValue.high)) {
            isSuccess = false;
        }

        if(!luaGetField(candleSubscription->luaState, dataSource, "L")) {
            logger->error("Could not get candle low price with class code: {}, ticker: {} and interval: {} because could not get lua field <<L>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK != result || !luaGetNumber(candleSubscription->luaState, &candleValue.low)) {
            isSuccess = false;
        }

        if(!luaGetField(candleSubscription->luaState, dataSource, "V")) {
            logger->error("Could not get candle volume with class code: {}, ticker: {} and interval: {} because could not get lua field <<V>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
        lua_pushvalue(candleSubscription->luaState, dataSource);
        lua_pushnumber(candleSubscription->luaState, candleCurrentIndex);
        result = lua_pcall(candleSubscription->luaState, 2, 1, 0);

        if (LUA_OK != result || !luaGetNumber(candleSubscription->luaState, &candleValue.volume)) {
            isSuccess = false;
        }

        if(!luaGetField(candleSubscription->luaState, dataSource, "T")) {
            logger->error("Could not get candle time with class code: {}, ticker: {} and interval: {} because could not get lua field <<T>>",
                candle->classCode, candle->ticker, candleSubscription->interval);
        };
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
        logger->warn("Could not convert candles to dto with the first index: {} because candles list starts from: {} so use: {} as first index",
            *candleFirstIndex, CANDLE_FIRST_INDEX, CANDLE_FIRST_INDEX);

        *candleFirstIndex = CANDLE_FIRST_INDEX;
    }

    if (*candleFirstIndex > candleLastIndex) {
        logger->warn("Could not convert candles to dto with the first index: {} because first index should be less or equal last index: {} so use last index as first index",
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
        return {};
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
    return {changedCandle};
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
