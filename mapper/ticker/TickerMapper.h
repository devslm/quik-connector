//
// Created by Sergey on 21.07.2021.
//

#ifndef QUIK_CONNECTOR_TICKERMAPPER_H
#define QUIK_CONNECTOR_TICKERMAPPER_H

#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../../mapper/date/DateMapper.h"
#include "../../dto/option/Option.h"
#include "../../dto/ticker/TickerDto.h"
#include "../../service/quik/utils/QuikUtils.h"

using namespace nlohmann;
using namespace std;

bool toTickerDto(lua_State *luaState, TickerDto *ticker);

json toTickerJson(Option<TickerDto>& tickerOption);

#endif //QUIK_CONNECTOR_TICKERMAPPER_H
