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

    if (!luaGetTableIntegerField(luaState, "trade_num", &trade->tradeNum)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "order_num", &trade->orderNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "flags", &trade->flags)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "price", &trade->price)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "qty", &trade->qty)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "value", &trade->value)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "accruedint", &trade->accruedInt)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "yield", &trade->yield)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "settlecode", &trade->settleCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "reporate", &trade->repoRate)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repovalue", &trade->repoValue)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repo2value", &trade->repo2value)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repoterm", &trade->repoTerm)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "sec_code", &trade->secCode)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_code", &trade->classCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "period", &trade->period)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "exchange_code", &trade->exchangeCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "clearing_comission", &trade->clearingComission)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "exchange_comission", &trade->exchangeComission)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "tech_center_comission", &trade->techCenterComission)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "broker_comission", &trade->brokerComission)) {
        return false;
    }
    if (!toDateMillis(luaState, "datetime", &trade->date)) {
        return false;
    }
    lua_pop(luaState, 1);

    return true;
}

string toTradeJson(Option<TradeDto> *tradeOption) {
    if (tradeOption->isEmpty()) {
        return "{}";
    }
    json jsonObject;
    TradeDto trade = tradeOption->get();
    jsonObject["tradeNum"] = trade.tradeNum;
    jsonObject["orderNum"] = trade.orderNum;
    jsonObject["flags"] = trade.flags;
    jsonObject["price"] = trade.price;
    jsonObject["qty"] = trade.qty;
    jsonObject["value"] = trade.value;
    jsonObject["accruedInt"] = trade.accruedInt;
    jsonObject["yield"] = trade.yield;
    jsonObject["settleCode"] = trade.settleCode;
    jsonObject["repoRate"] = trade.repoRate;
    jsonObject["repoValue"] = trade.repoValue;
    jsonObject["repo2value"] = trade.repo2value;
    jsonObject["repoTerm"] = trade.repoTerm;
    jsonObject["secCode"] = trade.secCode;
    jsonObject["classCode"] = trade.classCode;
    jsonObject["date"] = trade.date;
    jsonObject["period"] = trade.period;
    jsonObject["exchangeCode"] = trade.exchangeCode;
    jsonObject["clearingComission"] = trade.clearingComission;
    jsonObject["exchangeComission"] = trade.exchangeComission;
    jsonObject["techCenterComission"] = trade.techCenterComission;
    jsonObject["brokerComission"] = trade.brokerComission;

    return jsonObject.dump();
}
