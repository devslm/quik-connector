//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TRADEMAPPER_H
#define QUIK_CONNECTOR_TRADEMAPPER_H

#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/quik/trade/TradeDto.h"
#include "../date/DateMapper.h"
#include "../../../dto/option/Option.h"

using namespace nlohmann;

bool toAllTradeDto(lua_State *luaState, TradeDto *trade);

bool toTradeDto(lua_State *L, TradeDto *trade);

json toAllTradeJson(Option<TradeDto>& tradeOption);

json toTradeJson(Option<TradeDto>& tradeOption);

#endif //QUIK_CONNECTOR_TRADEMAPPER_H
