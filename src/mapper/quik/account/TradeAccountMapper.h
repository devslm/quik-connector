//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TRADEACCOUNTMAPPER_H
#define QUIK_CONNECTOR_TRADEACCOUNTMAPPER_H

#include <string>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/account/TradeAccountDto.h"
#include "../../../service/utils/string/StringUtils.h"

using namespace nlohmann;
using namespace std;

bool toTradeAccountDto(lua_State *luaState, TradeAccountDto* tradeAccount);

json toTradeAccountJson(const list<TradeAccountDto>& tradeAccounts);

json toTradeAccountJson(Option<TradeAccountDto>& tradeAccountOption);

#endif //QUIK_CONNECTOR_TRADEACCOUNTMAPPER_H
