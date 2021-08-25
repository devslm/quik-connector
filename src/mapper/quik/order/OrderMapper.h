//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERMAPPER_H
#define QUIK_CONNECTOR_ORDERMAPPER_H

#include <string>
#include <list>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../component/db/Db.h"
#include "../../../service/lua/Lua.h"
#include "../../../service/quik/Quik.h"
#include "../../../dto/quik/ticker/TickerDto.h"
#include "../date/DateMapper.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/order/OrderDto.h"
#include "../../../entity/quik/order/OrderEntity.h"
#include "../../../dto/quik/order/StopOrderDto.h"

using namespace nlohmann;
using namespace std;

class Quik;
class ConfigService;

bool toOrderDto(lua_State *luaState, Quik* quik, OrderDto* order);

bool toStopOrderDto(lua_State *luaState, Quik* quik, StopOrderDto* stopOrder);

OrderDto toOrderDto(OrderEntity& orderEntity);

OrderEntity toOrderEntity(OrderDto& order);

OrderEntity toOrderEntity(SQLite::Statement& query);

json toOrderJson(list<OrderDto>& orders);

json toOrderJson(Option<OrderDto>& orderOption);

json toStopOrderJson(list<StopOrderDto>& stopOrders);

json toStopOrderJson(Option<StopOrderDto>& stopOrderOption);

#endif //QUIK_CONNECTOR_ORDERMAPPER_H
