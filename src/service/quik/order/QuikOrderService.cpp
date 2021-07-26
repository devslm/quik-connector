//
// Created by Sergey on 21.07.2021.
//

#include "QuikOrderService.h"

QuikOrderService::QuikOrderService(Quik *quik) {
    this->quik = quik;
    this->mutexLock = luaGetMutex();
}

QuikOrderService::~QuikOrderService() {

}

static string buildOrderCacheKey(uint64_t orderId) {
    return "order:new:" + to_string(orderId);
}

list<OrderDto> QuikOrderService::getNewOrders(lua_State *luaState) {
    auto orders = getOrders(luaState);
    list<OrderDto> newOrders;

    LOGGER->debug("Found: {} orders before filtering new orders", orders.size());

    for (const auto& order : orders) {
        auto cacheKey = buildOrderCacheKey(order.orderNum);
        auto cacheOrderData = redis->getConnection().get(cacheKey);

        redis->getConnection().sync_commit();

        auto cacheOrderStatus = cacheOrderData.get();
        string cacheValue = order.status + " > " + to_string(order.balance);

        // If new order status and balance equals with cached value - skip it
        if (cacheOrderStatus.is_string()
                && cacheOrderStatus.as_string() == cacheValue) {
            LOGGER->debug("Skipping order: {} because already exists with the same status...", order.orderNum);
            continue;
        }
        redis->getConnection().setex(cacheKey, 2 * 24 * 60 * 60, cacheValue);
        redis->getConnection().sync_commit();

        newOrders.push_back(order);
    }
    return newOrders;
}

list<OrderDto> QuikOrderService::getOrders(lua_State *luaState) {
    list<OrderDto> orders;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{STRING_TYPE, QUIK_ORDERS_TABLE_NAME, 0, 0.0, false}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        LOGGER->error("Could not call QUIK {} function!", GET_NUMBER_OF_FUNCTION_NAME);
        return orders;
    }
    double totalOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalOrders);

    if (!isSuccess) {
        LOGGER->error("Could not get orders because can't retrieve total orders number!");
        return orders;
    }
    LOGGER->debug("Found: {} active orders", (int)totalOrders);

    for (int i = 0; i < totalOrders; ++i) {
        FunctionArgDto args[] = {
            {STRING_TYPE, QUIK_ORDERS_TABLE_NAME, 0, 0.0, false},
            {INTEGER_TYPE, "", i, 0.0, false}
        };

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, args)) {
            LOGGER->error("Could not call QUIK {} function!", GET_ITEM_FUNCTION_NAME);
            return orders;
        }
        OrderDto order;

        isSuccess = toOrderDto(luaState, quik, &order);

        if (!isSuccess) {
            LOGGER->error("Could not convert order data to dto!");
            continue;
        }

        if (!configService->getConfig().order.ignoreCancelled
                || order.status != ORDER_STATUS_CANCELED) {
            orders.push_back(order);
        } else {
            LOGGER->debug("Skipping add order: {} data to list because it status: {}", order.orderNum, order.status);
        }
    }
    return orders;
}
