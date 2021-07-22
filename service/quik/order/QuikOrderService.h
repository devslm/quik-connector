//
// Created by Sergey on 21.07.2021.
//

#ifndef QUIK_CONNECTOR_QUIKORDERSERVICE_H
#define QUIK_CONNECTOR_QUIKORDERSERVICE_H

#include <cstdint>
#include <mutex>
#include <string>
#include "../../../service/lua/Lua.h"
#include "../../../dto/config/Config.h"
#include "../../../service/quik/utils/QuikUtils.h"
#include "../../../service/lua/Lua.h"
#include "../../../mapper/order/OrderMapper.h"
#include "../../../dto/order/OrderDto.h"
#include "../../../service/quik/Quik.h"
#include "../../../component/redis/Redis.h"

using namespace std;

class ConfigService;
class Redis;

extern ConfigService *configService;
extern Redis* redis;

class QuikOrderService {
public:
    QuikOrderService(Quik *quik);

    virtual ~QuikOrderService();

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

private:
    recursive_mutex *mutexLock;
    Quik *quik;
};

#endif //QUIK_CONNECTOR_QUIKORDERSERVICE_H
