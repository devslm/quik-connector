//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikOrderService.h"

QuikOrderService::QuikOrderService(Quik *quik) {
    this->quik = quik;
    this->mutexLock = luaGetMutex();
}

static string buildOrderCacheKey(uint64_t orderId) {
    return "order:new:" + to_string(orderId);
}

void QuikOrderService::onNewOrder(OrderDto& order) {
    if (isOrderExistsInCache(order)) {
        logger->warn("Skipping save new order in onNewOrder callback because order: {} already exists!", order.orderNum);
        return;
    }
    list<OrderDto> newOrders = {order};

    saveNewOrder(newOrders);
}

list<OrderDto> QuikOrderService::getNewOrders(lua_State *luaState) {
    auto orders = getOrders(luaState);
    list<OrderDto> newOrders;

    logger->debug("Found: {} orders before filtering new orders", orders.size());

    for (const auto& order : orders) {
        auto isOrderExists = isOrderExistsInCache(order);

        if (isOrderExists) {
            logger->debug("Skipping order: {} because already exists with the same status...", order.orderNum);
            continue;
        }
        Option<OrderDto> orderOption(order);

        logger->info("New order: {}", toOrderJson(orderOption).dump());

        addOrderToCache(order);

        newOrders.push_back(order);
    }
    saveNewOrder(newOrders);

    return newOrders;
}

bool QuikOrderService::isOrderExistsInCache(const OrderDto& order) {
    auto cacheKey = buildOrderCacheKey(order.orderNum);
    auto cacheOrderData = redis->getConnection().get(cacheKey);

    redis->getConnection().sync_commit();

    auto cacheOrderStatus = cacheOrderData.get();
    auto cacheValue = order.status + " > " + to_string(order.balance);

    // If new order status and balance equals with cached value - skip it
    if (cacheOrderStatus.is_string()
            && cacheOrderStatus.as_string() == cacheValue) {
        return true;
    }
    return false;
}

void QuikOrderService::addOrderToCache(const OrderDto& order) {
    auto cacheKey = buildOrderCacheKey(order.orderNum);
    auto cacheValue = order.status + " > " + to_string(order.balance);

    redis->getConnection().setex(cacheKey, 2 * 24 * 60 * 60, cacheValue);
    redis->getConnection().sync_commit();
}

void QuikOrderService::saveNewOrder(list<OrderDto>& orders) {
    if (orders.empty()
            || !configService->getConfig().quik.order.saveToDb) {
        return;
    }

    try {
        list<OrderEntity> orderEntities;

        for (auto& order : orders) {
            orderEntities.push_back(
                toOrderEntity(order)
            );
        }
        OrderRepository::upsertAll(orderEntities);
    } catch (exception& exception) {
        logger->error("Could not save: {} new orders to DB! Reason: {}", orders.size(), exception.what());
    }
}

list<OrderDto> QuikOrderService::getOrders(lua_State *luaState) {
    list<OrderDto> orders;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{QUIK_ORDERS_TABLE_NAME}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_NUMBER_OF_FUNCTION_NAME);
        return orders;
    }
    double totalOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalOrders);

    if (!isSuccess) {
        logger->error("Could not get orders because can't retrieve total orders number!");
        return orders;
    }
    logger->debug("Found: {} active orders", (int)totalOrders);

    for (int i = 0; i < (int)totalOrders; ++i) {
        FunctionArgDto getItemFunctionArgs[] = {{QUIK_ORDERS_TABLE_NAME}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, getItemFunctionArgs)) {
            logger->error("Could not call QUIK {} function!", GET_ITEM_FUNCTION_NAME);
            return orders;
        }
        OrderDto order;

        isSuccess = toOrderDto(luaState, quik, &order);

        if (!isSuccess) {
            logger->error("Could not convert order data to dto on position: {} from total: {} in orders table!", i, totalOrders);
            continue;
        }

        if (!configService->getConfig().quik.order.ignoreCancelled
                || order.status != ORDER_STATUS_CANCELED) {
            orders.push_back(order);
        } else {
            logger->debug("Skipping add order: {} data to list because it's status: {}", order.orderNum, order.status);
        }
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return orders;
}

list<StopOrderDto> QuikOrderService::getStopOrders(lua_State *luaState) {
    list<StopOrderDto> stopOrders;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{QUIK_STOP_ORDERS_TABLE_NAME}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_NUMBER_OF_FUNCTION_NAME);
        return stopOrders;
    }
    double totalStopOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalStopOrders);

    if (!isSuccess) {
        logger->error("Could not get stop orders because can't retrieve total stop orders number!");
        return stopOrders;
    }
    logger->debug("Found: {} active stop orders", (int)totalStopOrders);

    for (int i = 0; i < totalStopOrders; ++i) {
        FunctionArgDto getItemFunctionArgs[] = {{QUIK_STOP_ORDERS_TABLE_NAME}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, getItemFunctionArgs)) {
            logger->error("Could not call QUIK {} function!", GET_ITEM_FUNCTION_NAME);
            return stopOrders;
        }
        StopOrderDto stopOrder;

        isSuccess = toStopOrderDto(luaState, quik, &stopOrder);

        if (!isSuccess) {
            logger->error("Could not convert stop order data to dto!");
            continue;
        }
        stopOrders.push_back(stopOrder);
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return stopOrders;
}

bool QuikOrderService::cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest) {
    logger->info(
        "Cancel stop order with id: {}, account: {}, clientCode: {}, class code: {} and ticker: {}",
         cancelStopOrderRequest.stopOrderId,
         cancelStopOrderRequest.account,
         cancelStopOrderRequest.clientCode,
         cancelStopOrderRequest.classCode,
         cancelStopOrderRequest.ticker
     );
    Option<string> validationError = validateCancelStopOrderData(cancelStopOrderRequest);

    if (validationError.isPresent()) {
        logger->info("Could not cancel stop order: {}! Reason: {}!", cancelStopOrderRequest.stopOrderId, validationError.get());
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

    if (!luaCallFunctionWithTableArg(luaState, SEND_TRANSACTION_FUNCTION_NAME, 1, 1, luaTable)) {
        logger->error(
            "Could not cancel stop order: {} because error on send transaction to QUIK!", cancelStopOrderRequest.stopOrderId
        );
        return false;
    }
    return true;
}
