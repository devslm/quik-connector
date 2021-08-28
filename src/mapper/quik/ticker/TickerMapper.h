//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TICKERMAPPER_H
#define QUIK_CONNECTOR_TICKERMAPPER_H

#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../date/DateMapper.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/ticker/TickerDto.h"
#include "../../../service/quik/utils/QuikUtils.h"

using namespace nlohmann;
using namespace std;

bool toTickerDto(lua_State *luaState, TickerDto *ticker);

json toTickerJson(const list<TickerDto>& tickers);

json toTickerJson(Option<TickerDto>& tickerOption);

#endif //QUIK_CONNECTOR_TICKERMAPPER_H
