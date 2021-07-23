//
// Created by Sergey on 21.07.2021.
//

#ifndef QUIK_CONNECTOR_ORDERMAPPER_H
#define QUIK_CONNECTOR_ORDERMAPPER_H

#include <string>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../../service/quik/Quik.h"
#include "../../dto/ticker/TickerDto.h"
#include "../date/DateMapper.h"
#include "../../dto/option/Option.h"
#include "../../dto/order/OrderDto.h"

using namespace nlohmann;
using namespace std;

class Quik;
class ConfigService;

extern ConfigService *configService;

bool toOrderDto(lua_State *luaState, Quik* quik, OrderDto* order);

json toOrderJson(list<OrderDto>& orders);

json toOrderJson(Option<OrderDto>& orderOption);

#endif //QUIK_CONNECTOR_ORDERMAPPER_H
