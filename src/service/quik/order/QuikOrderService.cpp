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

    FunctionArgDto args[] = {{QUIK_ORDERS_TABLE_NAME}};

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
        FunctionArgDto args[] = {{QUIK_ORDERS_TABLE_NAME}, {i}};

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

list<StopOrderDto> QuikOrderService::getStopOrders(lua_State *luaState) {
    list<StopOrderDto> stopOrders;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{QUIK_STOP_ORDERS_TABLE_NAME}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        LOGGER->error("Could not call QUIK {} function!", GET_NUMBER_OF_FUNCTION_NAME);
        return stopOrders;
    }
    double totalStopOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalStopOrders);

    if (!isSuccess) {
        LOGGER->error("Could not get stop orders because can't retrieve total stop orders number!");
        return stopOrders;
    }
    LOGGER->debug("Found: {} active stop orders", (int)totalStopOrders);

    for (int i = 0; i < totalStopOrders; ++i) {
        FunctionArgDto args[] = {{QUIK_STOP_ORDERS_TABLE_NAME}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, args)) {
            LOGGER->error("Could not call QUIK {} function!", GET_ITEM_FUNCTION_NAME);
            return stopOrders;
        }
        StopOrderDto stopOrder;

        isSuccess = toStopOrderDto(luaState, quik, &stopOrder);

        if (!isSuccess) {
            LOGGER->error("Could not convert stop order data to dto!");
            continue;
        }
        stopOrders.push_back(stopOrder);
    }
    return stopOrders;
}

bool QuikOrderService::cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest) {
    LOGGER->info(
        "Cancel stop order with id: {}, account: {}, clientCode: {}, class code: {} and ticker: {}",
         cancelStopOrderRequest.stopOrderId,
         cancelStopOrderRequest.account,
         cancelStopOrderRequest.clientCode,
         cancelStopOrderRequest.classCode,
         cancelStopOrderRequest.ticker
     );
    Option<string> validationError = validateCancelStopOrderData(cancelStopOrderRequest);

    if (validationError.isPresent()) {
        LOGGER->info("Could not cancel stop order: {}! Reason: {}!", cancelStopOrderRequest.stopOrderId, validationError.get());
        return false;
    }
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    map<string, string> luaTable;
    luaTable["ACTION"] = "KILL_STOP_ORDER";
    luaTable["ACCOUNT"] = cancelStopOrderRequest.account;
    luaTable["CLIENT_CODE"] = cancelStopOrderRequest.clientCode;
    luaTable["TRANS_ID"] = to_string(QuikUtils::newTransactionId());
    luaTable["CLASSCODE"] = cancelStopOrderRequest.classCode;
    luaTable["SECCODE"] = cancelStopOrderRequest.ticker;
    luaTable["STOP_ORDER_KEY"] = to_string(cancelStopOrderRequest.stopOrderId);

    return true;
}
