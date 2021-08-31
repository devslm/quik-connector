//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "TradeMapper.h"

using namespace std;

bool toAllTradeDto(lua_State *luaState, TradeDto *trade) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for all trade data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }

    if (!luaGetTableIntegerField(luaState, "trade_num", &trade->tradeNum)) {
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
    if (!luaGetTableStringField(luaState, "sec_code", &trade->ticker)) {
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
    if (!toDateMillis(luaState, "datetime", &trade->date)) {
        return false;
    }
    lua_pop(luaState, 1);

    return true;
}

bool toTradeDto(lua_State *luaState, TradeDto *trade) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for trade data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        lua_pop(luaState, 1);

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
    if (!luaGetTableStringField(luaState, "sec_code", &trade->ticker)) {
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

json toAllTradeJson(Option<TradeDto>& tradeOption) {
    json jsonObject;

    if (tradeOption.isEmpty()) {
        return jsonObject;
    }
    TradeDto trade = tradeOption.get();
    jsonObject["tradeNum"] = trade.tradeNum;
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
    jsonObject["ticker"] = trade.ticker;
    jsonObject["classCode"] = trade.classCode;
    jsonObject["date"] = trade.date;
    jsonObject["period"] = trade.period;
    jsonObject["exchangeCode"] = trade.exchangeCode;

    return jsonObject;
}

json toTradeJson(Option<TradeDto>& tradeOption) {
    json jsonObject = toAllTradeJson(tradeOption);

    if (tradeOption.isEmpty()) {
        return jsonObject;
    }
    TradeDto trade = tradeOption.get();
    jsonObject["orderNum"] = trade.orderNum;
    jsonObject["clearingComission"] = trade.clearingComission;
    jsonObject["exchangeComission"] = trade.exchangeComission;
    jsonObject["techCenterComission"] = trade.techCenterComission;
    jsonObject["brokerComission"] = trade.brokerComission;

    return jsonObject;
}
