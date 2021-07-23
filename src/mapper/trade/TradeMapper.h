//
// Created by Sergey on 25.06.2021.
//

#ifndef QUIK_CONNECTOR_TRADEMAPPER_H
#define QUIK_CONNECTOR_TRADEMAPPER_H

#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../../dto/trade/TradeDto.h"
#include "../date/DateMapper.h"
#include "../../dto/option/Option.h"

using namespace nlohmann;

bool toTradeDto(lua_State *L, TradeDto *trade);

string toTradeJson(Option<TradeDto> *tradeOption);

#endif //QUIK_CONNECTOR_TRADEMAPPER_H
