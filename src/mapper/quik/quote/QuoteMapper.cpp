//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuoteMapper.h"

// TODO Refactoring required
bool toTickerQuoteDto(lua_State *luaState, TickerQuoteDto *tickerQuote) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for ticker quotes data! Current stack value type is: <<{}>> but required table!",
             luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }

    if (!luaGetField(luaState, -1, "bid_count")) {
        return false;
    }
    tickerQuote->bidCount = lua_tonumber(luaState, -1);
    lua_pop(luaState, 1);

    if (!luaGetField(luaState, -1, "offer_count")) {
        return false;
    }
    tickerQuote->offerCount = lua_tonumber(luaState, -1);
    lua_pop(luaState, 1);

    if (!luaGetField(luaState, -1, "bid")) {
        return false;
    }
    size_t totalBids = lua_rawlen(luaState, -1);

    for (int currentIndex = 1; currentIndex <= totalBids; ++currentIndex) {
        lua_pushinteger(luaState, currentIndex);
        lua_gettable(luaState, -2);

        if (lua_type(luaState, -1) == LUA_TNIL) {
            lua_pop(luaState, 1);
            break;
        }
        TickerPriceDto tickerPrice;

        if (!luaGetField(luaState, -1, "price")) {
            return false;
        }
        tickerPrice.price = lua_tonumber(luaState, -1);
        lua_pop(luaState, 1);

        if (!luaGetField(luaState, -1, "quantity")) {
            return false;
        }
        tickerPrice.quantity = lua_tonumber(luaState, -1);
        lua_pop(luaState, 2);

        tickerQuote->bids.push_back(tickerPrice);
    }
    lua_pop(luaState, 1);

    if (!luaGetField(luaState, -1, "offer")) {
        return false;
    }
    size_t totalOffers = lua_rawlen(luaState, -1);

    for (int currentIndex = 1; currentIndex <= totalOffers; ++currentIndex) {
        lua_pushinteger(luaState, currentIndex);
        lua_gettable(luaState, -2);

        if (lua_type(luaState, -1) == LUA_TNIL) {
            lua_pop(luaState, 1);
            break;
        }
        TickerPriceDto tickerPrice;

        if (!luaGetField(luaState, -1, "price")) {
            return false;
        }
        tickerPrice.price = lua_tonumber(luaState, -1);
        lua_pop(luaState, 1);

        if (!luaGetField(luaState, -1, "quantity")) {
            return false;
        }
        tickerPrice.quantity = lua_tonumber(luaState, -1);
        lua_pop(luaState, 2);

        tickerQuote->offers.push_back(tickerPrice);
    }
    lua_pop(luaState, 2);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

json toTickerQuoteJson(Option<TickerQuoteDto>& tickerQuoteOption) {
    json jsonObject;

    if (tickerQuoteOption.isEmpty()) {
        return jsonObject;
    }
    const auto tickerQuote = tickerQuoteOption.get();

    jsonObject["classCode"] = tickerQuote.classCode;
    jsonObject["ticker"] = tickerQuote.ticker;
    jsonObject["bidCount"] = tickerQuote.bidCount;
    jsonObject["offerCount"] = tickerQuote.offerCount;
    jsonObject["bids"] = json::array();
    jsonObject["offers"] = json::array();

    for (const auto& bid : tickerQuote.bids) {
        json valuesJson;
        valuesJson["price"] = bid.price;
        valuesJson["quantity"] = bid.quantity;

        jsonObject["bids"].push_back(valuesJson);
    }

    for (const auto& offer : tickerQuote.offers) {
        json valuesJson;
        valuesJson["price"] = offer.price;
        valuesJson["quantity"] = offer.quantity;

        jsonObject["offers"].push_back(valuesJson);
    }
    return jsonObject;
}
