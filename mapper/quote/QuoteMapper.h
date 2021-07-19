//
// Created by Sergey on 15.07.2021.
//

#ifndef QUIK_CONNECTOR_QUOTEMAPPER_H
#define QUIK_CONNECTOR_QUOTEMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../../dto/option/Option.h"
#include "../../dto/quote/TickerQuoteDto.h"

using namespace nlohmann;
using namespace std;

bool toTickerQuoteDto(lua_State *luaState, TickerQuoteDto *tickerQuote);

json toTickerQuoteJson(Option<TickerQuoteDto> *tickerQuoteOption);

#endif //QUIK_CONNECTOR_QUOTEMAPPER_H
