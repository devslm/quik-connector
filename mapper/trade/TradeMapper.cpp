//
// Created by Sergey on 25.06.2021.
//

#include "TradeMapper.h"

using namespace std;

bool toTradeDto(lua_State *luaState, TradeDto *trade) {
    if (!lua_istable(luaState, -1)) {
        logError("Could not get table for trade data! Current stack value type is: <<%s>> but required table!", luaGetType(luaState, -1));

        return false;
    }
    bool isSuccess = true;

    if (!luaGetTableNumberField(luaState, "trade_num", &trade->tradeNum)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "flags", &trade->flags)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "price", &trade->price)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "qty", &trade->qty)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "value", &trade->value)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "accruedint", &trade->accruedint)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "yield", &trade->yield)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "settlecode", &trade->settlecode)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "reporate", &trade->reporate)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "repovalue", &trade->repovalue)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "repo2value", &trade->repo2value)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "repoterm", &trade->repoterm)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "sec_code", &trade->secCode)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "class_code", &trade->classCode)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "period", &trade->period)) {
        isSuccess = false;
    }

    if (!luaGetTableNumberField(luaState, "open_interest", &trade->openInterest)) {
        isSuccess = false;
    }

    if (!luaGetTableStringField(luaState, "exchange_code", &trade->exchangeCode)) {
        isSuccess = false;
    }

    if (!toDateMillis(luaState, "datetime", &trade->date)) {
        isSuccess = false;
    }
    return isSuccess;
}

string toTradeJson(Option<TradeDto> *tradeOption) {
    if (tradeOption->isEmpty()) {
        return "{}";
    }
    json jsonObject;
    TradeDto trade = tradeOption->get();

    jsonObject["tradeNum"] = trade.tradeNum;
    jsonObject["flags"] = trade.flags;
    jsonObject["price"] = trade.price;
    jsonObject["qty"] = trade.qty;
    jsonObject["value"] = trade.value;
    jsonObject["accruedint"] = trade.accruedint;
    jsonObject["yield"] = trade.yield;
    jsonObject["settlecode"] = trade.settlecode;
    jsonObject["reporate"] = trade.reporate;
    jsonObject["repovalue"] = trade.repovalue;
    jsonObject["repo2value"] = trade.repo2value;
    jsonObject["repoterm"] = trade.repoterm;
    jsonObject["secCode"] = trade.secCode;
    jsonObject["classCode"] = trade.classCode;
    jsonObject["date"] = trade.date;
    jsonObject["period"] = trade.period;
    jsonObject["openInterest"] = trade.openInterest;
    jsonObject["exchangeCode"] = trade.exchangeCode;

    return jsonObject.dump();
}
