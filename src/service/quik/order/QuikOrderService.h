//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKORDERSERVICE_H
#define QUIK_CONNECTOR_QUIKORDERSERVICE_H

#include <cstdint>
#include <mutex>
#include <string>
#include "../../lua/Lua.h"
#include "../../../dto/config/Config.h"
#include "../utils/QuikUtils.h"
#include "../../../mapper/quik/order/OrderMapper.h"
#include "../../../dto/quik/order/OrderDto.h"
#include "../../../dto/quik/order/StopOrderDto.h"
#include "../Quik.h"
#include "../../../component/redis/Redis.h"
#include "../../../component/quik/validator/order/OrderValidator.h"
#include "../../../repository/quik/order/OrderRepository.h"

using namespace std;

class ConfigService;
class Redis;

class QuikOrderService {
public:
    QuikOrderService(Quik *quik);

    ~QuikOrderService() = default;

    void onNewOrder(OrderDto& order);

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

    list<StopOrderDto> getStopOrders(lua_State *luaState);

    bool cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest);

private:
    recursive_mutex *mutexLock;
    Quik *quik;

    bool isOrderExistsInCache(const OrderDto& order);

    void addOrderToCache(const OrderDto& order);

    void saveNewOrder(list<OrderDto>& orders);
};

#endif //QUIK_CONNECTOR_QUIKORDERSERVICE_H
