//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKORDERSERVICE_H
#define QUIK_CONNECTOR_QUIKORDERSERVICE_H

#include <cstdint>
#include <mutex>
#include <string>
#include "../../lua/Lua.h"
#include "../../../dto/config/Config.h"
#include "../utils/QuikUtils.h"
#include "../../lua/Lua.h"
#include "../../../mapper/quik/order/OrderMapper.h"
#include "../../../dto/quik/order/OrderDto.h"
#include "../../../dto/quik/order/StopOrderDto.h"
#include "../Quik.h"
#include "../../../component/redis/Redis.h"
#include "../../../component/quik/validator/order/OrderValidator.h"

using namespace std;

class ConfigService;
class Redis;

class QuikOrderService {
public:
    QuikOrderService(Quik *quik);

    ~QuikOrderService() = default;

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

    list<StopOrderDto> getStopOrders(lua_State *luaState);

    bool cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest);

private:
    recursive_mutex *mutexLock;
    Quik *quik;
};

#endif //QUIK_CONNECTOR_QUIKORDERSERVICE_H
