//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_MONEYLIMITMAPPER_H
#define QUIK_CONNECTOR_MONEYLIMITMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/money/MoneyLimitDto.h"

using namespace nlohmann;
using namespace std;

bool toMoneyLimitDto(lua_State *luaState, MoneyLimitDto* moneyLimit);

json toMoneyLimitJson(Option<MoneyLimitDto>& moneyLimitOption);

#endif //QUIK_CONNECTOR_MONEYLIMITMAPPER_H
