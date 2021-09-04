//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "OrderMapper.h"

static Option<TradeDto> getTradeByOrderId(uint64_t orderId, const list<TradeDto>& trades);

static void addOrderCommissionData(lua_State *luaState, Quik *quik, OrderDto* order);

bool toOrderDto(lua_State *luaState, Quik *quik, OrderDto* order) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for order data! Current stack value type is: <<{}>> but required table!",
             luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }

    if (!luaGetTableIntegerField(luaState, "order_num", &order->orderNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "flags", &order->flags)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "brokerref", &order->brokerRef)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "userid", &order->userId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "firmid", &order->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "account", &order->account)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "price", &order->price)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "qty", &order->qty)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "balance", &order->balance)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "value", &order->value)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "accruedint", &order->accruedInt)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "yield", &order->yield)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "trans_id", &order->transId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "client_code", &order->clientCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "price2", &order->price2)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "settlecode", &order->settleCode)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "uid", &order->uid)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "canceled_uid", &order->canceledUid)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "exchange_code", &order->exchangeCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "activation_time", &order->activationTime)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "linkedorder", &order->linkedOrder)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "expiry", &order->expiry)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "sec_code", &order->ticker)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_code", &order->classCode)) {
        return false;
    }
    if (!toDateMillis(luaState, "datetime", &order->date)) {
        return false;
    }
    if (!toDateMillis(luaState, "withdraw_datetime", &order->withdrawDate)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "bank_acc_id", &order->bankAccId)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "value_entry_type", &order->valueEntryType)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repoterm", &order->repoTerm)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repovalue", &order->repoValue)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repo2value", &order->repo2value)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "repo_value_balance", &order->repoValueBalance)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "start_discount", &order->startDiscount)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "reject_reason", &order->rejectReason)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "ext_order_flags", &order->extOrderFlags)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "min_qty", &order->minQty)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "exec_type", &order->execType)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "side_qualifier", &order->sideQualifier)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "acnt_type", &order->acntType)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "capacity", &order->capacity)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "passive_only_order", &order->passiveOnlyOrder)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "visible", &order->visible)) {
        return false;
    }
    order->status = QuikUtils::getOrderStatus(order->flags);
    order->type = QuikUtils::getOrderType(order->flags);
    Option<string> classType = QuikUtils::getClassTypeByCode(order->classCode);

    if (!classType.isPresent()) {
        logger->error("Could not get order class type with class code: {}", order->classCode);
        return false;
    }
    order->classType = classType.get();

    addOrderCommissionData(luaState, quik, order);

    Option<TickerDto> tickerOption = quik->getTickerById(luaState, order->classCode, order->ticker);

    if (tickerOption.isEmpty()) {
        logger->error("Could not convert order data to dto! Reason: Can't get ticker data with class code: {} and ticker: {}",
            order->classCode, order->ticker);
        return false;
    }
    TickerDto ticker = tickerOption.get();
    order->currency = ticker.faceUnit;
    order->name = ticker.shortName;
    order->lotSize = ticker.lotSize;
    Option<double> priceStepCostOption = quik->getTickerPriceStepCost(luaState, order->classCode, order->ticker);

    if (priceStepCostOption.isEmpty()) {
        logger->error("Could not convert order data to dto! Reason: Can't get ticker price step cost with class code: {} and ticker: {}",
            order->classCode, order->ticker);
        return false;
    }
    order->priceStepCost = priceStepCostOption.get();

    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

static Option<TradeDto> getTradeByOrderId(uint64_t orderId, const list<TradeDto>& trades) {
    logger->debug("Get trade data with order: {}", orderId);

    for (const auto& trade : trades) {
        if (trade.orderNum == orderId) {
            return {trade};
        }
    }
    return {};
}

static void addOrderCommissionData(lua_State *luaState, Quik *quik, OrderDto* order) {
    if (configService->getConfig().quik.order.ignoreCancelled
            && order->status == ORDER_STATUS_CANCELED) {
        logger->debug("Skipping add commission data to order: {} because it status: {}", order->orderNum, order->status);
        return;
    }
    list<TradeDto> trades = quik->getTrades(luaState);
    Option<TradeDto> tradeOption = getTradeByOrderId(order->orderNum, trades);

    if (tradeOption.isPresent()) {
        TradeDto trade = tradeOption.get();
        order->commission.broker = trade.brokerComission;
        order->commission.clearing = trade.clearingComission;
        order->commission.exchange = trade.exchangeComission;
        order->commission.techCenter = trade.techCenterComission;
    }
}

bool toStopOrderDto(lua_State *luaState, Quik* quik, StopOrderDto* stopOrder) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for stop order data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }

    if (!luaGetTableIntegerField(luaState, "order_num", &stopOrder->orderNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "flags", &stopOrder->flags)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "brokerref", &stopOrder->brokerRef)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "firmid", &stopOrder->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "account", &stopOrder->account)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "condition", &stopOrder->condition)) {
        return false;
    }
    stopOrder->conditionType = QuikUtils::getStopOrderConditionType(stopOrder->condition);

    if (!luaGetTableNumberField(luaState, "condition_price", &stopOrder->conditionPrice)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "price", &stopOrder->price)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "qty", &stopOrder->qty)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "linkedorder", &stopOrder->linkedOrder)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "expiry", &stopOrder->expiry)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "trans_id", &stopOrder->transId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "client_code", &stopOrder->clientCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "co_order_num", &stopOrder->coOrderNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "co_order_price", &stopOrder->coOrderPrice)) {
        return false;
    }
    uint64_t stopOrderType;

    if (!luaGetTableIntegerField(luaState, "stop_order_type", &stopOrderType)) {
        return false;
    }
    stopOrder->type = QuikUtils::getStopOrderType(stopOrderType);

    if (!luaGetTableIntegerField(luaState, "orderdate", &stopOrder->orderDate)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "alltrade_num", &stopOrder->allTradeNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "stopflags", &stopOrder->stopFlags)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "offset", &stopOrder->offset)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "spread", &stopOrder->spread)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "balance", &stopOrder->balance)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "uid", &stopOrder->uid)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "filled_qty", &stopOrder->filledQty)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "withdraw_time", &stopOrder->withdrawTime)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "condition_price2", &stopOrder->conditionPrice2)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "active_from_time", &stopOrder->activeFromTime)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "active_to_time", &stopOrder->activeToTime)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "sec_code", &stopOrder->ticker)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_code", &stopOrder->classCode)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "condition_sec_code", &stopOrder->conditionTicker)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "condition_class_code", &stopOrder->conditionClassCode)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "canceled_uid", &stopOrder->canceledUid)) {
        return false;
    }
    if (!toDateMillis(luaState, "order_date_time", &stopOrder->orderDateTime)) {
        return false;
    }
    if (!toDateMillis(luaState, "withdraw_datetime", &stopOrder->withdrawDate)) {
        return false;
    }
    if (!toDateMillis(luaState, "activation_date_time", &stopOrder->activationDate)) {
        return false;
    }
    stopOrder->status = QuikUtils::getOrderStatus(stopOrder->flags);
    Option<string> classType = QuikUtils::getClassTypeByCode(stopOrder->classCode);

    if (!classType.isPresent()) {
        logger->error("Could not get stop order class type with class code: {}", stopOrder->classCode);
        return false;
    }
    stopOrder->classType = classType.get();

    Option<TickerDto> tickerOption = quik->getTickerById(luaState, stopOrder->classCode, stopOrder->ticker);

    if (!tickerOption.isPresent()) {
        logger->error("Could not convert stop order data to dto! Reason: Can't get ticker data with class code: {} and ticker: {}",
            stopOrder->classCode, stopOrder->ticker);
        return false;
    }
    TickerDto ticker = tickerOption.get();
    stopOrder->currency = ticker.faceUnit;
    stopOrder->name = ticker.shortName;
    stopOrder->lotSize = ticker.lotSize;

    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

OrderDto toOrderDto(OrderEntity& orderEntity) {
    OrderDto order;
    order.orderNum = orderEntity.orderNum;
    order.flags = orderEntity.flags;
    order.brokerRef = orderEntity.brokerRef;
    order.userId = orderEntity.userId;
    order.firmId = orderEntity.firmId;
    order.account = orderEntity.account;
    order.price = orderEntity.price;
    order.qty = orderEntity.qty;
    order.balance = orderEntity.balance;
    order.value = orderEntity.value;
    order.accruedInt = orderEntity.accruedInt;
    order.yield = orderEntity.yield;
    order.transId = orderEntity.transId;
    order.clientCode = orderEntity.clientCode;
    order.price2 = orderEntity.price2;
    order.settleCode = orderEntity.settleCode;
    order.uid = orderEntity.uid;
    order.canceledUid = orderEntity.canceledUid;
    order.exchangeCode = orderEntity.exchangeCode;
    order.activationTime = orderEntity.activationTime;
    order.linkedOrder = orderEntity.linkedOrder;
    order.expiry = orderEntity.expiry;
    order.ticker = orderEntity.ticker;
    order.name = orderEntity.name;
    order.classCode = orderEntity.classCode;
    order.classType = orderEntity.classType;
    order.status = orderEntity.status;
    order.type = orderEntity.type;
    order.currency = orderEntity.currency;
    order.lotSize = orderEntity.lotSize;
    order.date = orderEntity.date;
    order.withdrawDate = orderEntity.withdrawDate;
    order.bankAccId = orderEntity.bankAccId;
    order.valueEntryType = orderEntity.valueEntryType;
    order.repoTerm = orderEntity.repoTerm;
    order.repoValue = orderEntity.repoValue;
    order.repo2value = orderEntity.repo2value;
    order.repoValueBalance = orderEntity.repoValueBalance;
    order.startDiscount = orderEntity.startDiscount;
    order.rejectReason = orderEntity.rejectReason;
    order.extOrderFlags = orderEntity.extOrderFlags;
    order.minQty = orderEntity.minQty;
    order.execType = orderEntity.execType;
    order.sideQualifier = orderEntity.sideQualifier;
    order.acntType = orderEntity.acntType;
    order.capacity = orderEntity.capacity;
    order.passiveOnlyOrder = orderEntity.passiveOnlyOrder;
    order.visible = orderEntity.visible;
    order.priceStepCost = orderEntity.priceStepCost;
    order.commission.broker = orderEntity.commission.broker;
    order.commission.clearing = orderEntity.commission.clearing;
    order.commission.techCenter = orderEntity.commission.techCenter;
    order.commission.exchange = orderEntity.commission.exchange;

    return order;
}

OrderEntity toOrderEntity(OrderDto& order) {
    OrderEntity orderEntity;
    orderEntity.orderNum = order.orderNum;
    orderEntity.flags = order.flags;
    orderEntity.brokerRef = order.brokerRef;
    orderEntity.userId = order.userId;
    orderEntity.firmId = order.firmId;
    orderEntity.account = order.account;
    orderEntity.price = order.price;
    orderEntity.qty = order.qty;
    orderEntity.balance = order.balance;
    orderEntity.value = order.value;
    orderEntity.accruedInt = order.accruedInt;
    orderEntity.yield = order.yield;
    orderEntity.transId = order.transId;
    orderEntity.clientCode = order.clientCode;
    orderEntity.price2 = order.price2;
    orderEntity.settleCode = order.settleCode;
    orderEntity.uid = order.uid;
    orderEntity.canceledUid = order.canceledUid;
    orderEntity.exchangeCode = order.exchangeCode;
    orderEntity.activationTime = order.activationTime;
    orderEntity.linkedOrder = order.linkedOrder;
    orderEntity.expiry = order.expiry;
    orderEntity.ticker = order.ticker;
    orderEntity.name = order.name;
    orderEntity.classCode = order.classCode;
    orderEntity.classType = order.classType;
    orderEntity.status = order.status;
    orderEntity.type = order.type;
    orderEntity.currency = order.currency;
    orderEntity.lotSize = order.lotSize;
    orderEntity.date = order.date;
    orderEntity.withdrawDate = order.withdrawDate;
    orderEntity.bankAccId = order.bankAccId;
    orderEntity.valueEntryType = order.valueEntryType;
    orderEntity.repoTerm = order.repoTerm;
    orderEntity.repoValue = order.repoValue;
    orderEntity.repo2value = order.repo2value;
    orderEntity.repoValueBalance = order.repoValueBalance;
    orderEntity.startDiscount = order.startDiscount;
    orderEntity.rejectReason = order.rejectReason;
    orderEntity.extOrderFlags = order.extOrderFlags;
    orderEntity.minQty = order.minQty;
    orderEntity.execType = order.execType;
    orderEntity.sideQualifier = order.sideQualifier;
    orderEntity.acntType = order.acntType;
    orderEntity.capacity = order.capacity;
    orderEntity.passiveOnlyOrder = order.passiveOnlyOrder;
    orderEntity.visible = order.visible;
    orderEntity.priceStepCost = order.priceStepCost;
    orderEntity.commission.broker = order.commission.broker;
    orderEntity.commission.clearing = order.commission.clearing;
    orderEntity.commission.techCenter = order.commission.techCenter;
    orderEntity.commission.exchange = order.commission.exchange;

    return orderEntity;
}

OrderEntity toOrderEntity(SQLite::Statement& query) {
    OrderEntity orderEntity;

    int columnPosition = 0;
    orderEntity.orderNum = query.getColumn(columnPosition++).getInt64();
    orderEntity.flags = query.getColumn(columnPosition++).getDouble();
    orderEntity.brokerRef = query.getColumn(columnPosition++).getString();
    orderEntity.userId = query.getColumn(columnPosition++).getString();
    orderEntity.firmId = query.getColumn(columnPosition++).getString();
    orderEntity.account = query.getColumn(columnPosition++).getString();
    orderEntity.price = query.getColumn(columnPosition++).getDouble();
    orderEntity.qty = query.getColumn(columnPosition++).getDouble();
    orderEntity.balance = query.getColumn(columnPosition++).getDouble();
    orderEntity.value = query.getColumn(columnPosition++).getDouble();
    orderEntity.accruedInt = query.getColumn(columnPosition++).getDouble();
    orderEntity.yield = query.getColumn(columnPosition++).getDouble();
    orderEntity.transId = query.getColumn(columnPosition++).getInt64();
    orderEntity.clientCode = query.getColumn(columnPosition++).getString();
    orderEntity.price2 = query.getColumn(columnPosition++).getDouble();
    orderEntity.settleCode = query.getColumn(columnPosition++).getString();
    orderEntity.uid = query.getColumn(columnPosition++).getInt64();
    orderEntity.canceledUid = query.getColumn(columnPosition++).getInt64();
    orderEntity.exchangeCode = query.getColumn(columnPosition++).getString();
    orderEntity.activationTime = query.getColumn(columnPosition++).getDouble();
    orderEntity.linkedOrder = query.getColumn(columnPosition++).getInt64();
    orderEntity.expiry = query.getColumn(columnPosition++).getDouble();
    orderEntity.ticker = query.getColumn(columnPosition++).getString();
    orderEntity.name = query.getColumn(columnPosition++).getString();
    orderEntity.classCode = query.getColumn(columnPosition++).getString();
    orderEntity.classType = query.getColumn(columnPosition++).getString();
    orderEntity.status = query.getColumn(columnPosition++).getString();
    orderEntity.type = query.getColumn(columnPosition++).getString();
    orderEntity.currency = query.getColumn(columnPosition++).getString();
    orderEntity.lotSize = query.getColumn(columnPosition++).getDouble();
    orderEntity.date = query.getColumn(columnPosition++).getInt64();
    orderEntity.withdrawDate = query.getColumn(columnPosition++).getInt64();
    orderEntity.bankAccId = query.getColumn(columnPosition++).getString();
    orderEntity.valueEntryType = query.getColumn(columnPosition++).getInt64();
    orderEntity.repoTerm = query.getColumn(columnPosition++).getDouble();
    orderEntity.repoValue = query.getColumn(columnPosition++).getDouble();
    orderEntity.repo2value = query.getColumn(columnPosition++).getDouble();
    orderEntity.repoValueBalance = query.getColumn(columnPosition++).getDouble();
    orderEntity.startDiscount = query.getColumn(columnPosition++).getDouble();
    orderEntity.rejectReason = query.getColumn(columnPosition++).getString();
    orderEntity.extOrderFlags = query.getColumn(columnPosition++).getDouble();
    orderEntity.minQty = query.getColumn(columnPosition++).getDouble();
    orderEntity.execType = query.getColumn(columnPosition++).getInt64();
    orderEntity.sideQualifier = query.getColumn(columnPosition++).getDouble();
    orderEntity.acntType = query.getColumn(columnPosition++).getDouble();
    orderEntity.capacity = query.getColumn(columnPosition++).getDouble();
    orderEntity.passiveOnlyOrder = query.getColumn(columnPosition++).getDouble();
    orderEntity.visible = query.getColumn(columnPosition++).getDouble();
    orderEntity.priceStepCost = query.getColumn(columnPosition++).getDouble();
    orderEntity.commission.broker = query.getColumn(columnPosition++).getDouble();
    orderEntity.commission.clearing = query.getColumn(columnPosition++).getDouble();
    orderEntity.commission.techCenter = query.getColumn(columnPosition++).getDouble();
    orderEntity.commission.exchange = query.getColumn(columnPosition++).getDouble();

    return orderEntity;
}

json toOrderJson(list<OrderDto>& orders) {
    json jsonArray = json::array();

    if (orders.empty()) {
        return jsonArray;
    }

    for (const auto& order : orders) {
        Option<OrderDto> orderOption(order);

        jsonArray.push_back(
            toOrderJson(orderOption)
        );
    }
    return jsonArray;
}

json toOrderJson(Option<OrderDto>& orderOption) {
    json jsonObject;

    if (orderOption.isEmpty()) {
        return jsonObject;
    }
    OrderDto order = orderOption.get();
    jsonObject["orderNum"] = order.orderNum;
    jsonObject["flags"] = order.flags;
    jsonObject["brokerRef"] = order.brokerRef;
    jsonObject["userId"] = order.userId;
    jsonObject["firmId"] = order.firmId;
    jsonObject["account"] = order.account;
    jsonObject["price"] = order.price;
    jsonObject["qty"] = order.qty;
    jsonObject["balance"] = order.balance;
    jsonObject["value"] = order.value;
    jsonObject["accruedInt"] = order.accruedInt;
    jsonObject["yield"] = order.yield;
    jsonObject["transId"] = order.transId;
    jsonObject["clientCode"] = order.clientCode;
    jsonObject["price2"] = order.price2;
    jsonObject["settleCode"] = order.settleCode;
    jsonObject["uid"] = order.uid;
    jsonObject["canceledUid"] = order.canceledUid;
    jsonObject["exchangeCode"] = order.exchangeCode;
    jsonObject["activationTime"] = order.activationTime;
    jsonObject["linkedOrder"] = order.linkedOrder;
    jsonObject["expiry"] = order.expiry;
    jsonObject["ticker"] = order.ticker;
    jsonObject["classCode"] = order.classCode;
    jsonObject["classType"] = order.classType;
    jsonObject["status"] = order.status;
    jsonObject["currency"] = order.currency;
    jsonObject["name"] = order.name;
    jsonObject["type"] = order.type;
    jsonObject["lotSize"] = order.lotSize;
    jsonObject["date"] = order.date;
    jsonObject["withdrawDate"] = order.withdrawDate;
    jsonObject["bankAccId"] = order.bankAccId;
    jsonObject["valueEntryType"] = order.valueEntryType;
    jsonObject["repoTerm"] = order.repoTerm;
    jsonObject["repoValue"] = order.repoValue;
    jsonObject["repo2value"] = order.repo2value;
    jsonObject["repoValueBalance"] = order.repoValueBalance;
    jsonObject["startDiscount"] = order.startDiscount;
    jsonObject["rejectReason"] = order.rejectReason;
    jsonObject["extOrderFlags"] = order.extOrderFlags;
    jsonObject["minQty"] = order.minQty;
    jsonObject["execType"] = order.execType;
    jsonObject["sideQualifier"] = order.sideQualifier;
    jsonObject["acntType"] = order.acntType;
    jsonObject["capacity"] = order.capacity;
    jsonObject["passiveOnlyOrder"] = order.passiveOnlyOrder;
    jsonObject["visible"] = order.visible;
    jsonObject["priceStepCost"] = order.priceStepCost;

    jsonObject["commission"] = json::object();
    jsonObject["commission"]["broker"] = order.commission.broker;
    jsonObject["commission"]["clearing"] = order.commission.clearing;
    jsonObject["commission"]["exchange"] = order.commission.exchange;
    jsonObject["commission"]["techCenter"] = order.commission.techCenter;

    return jsonObject;
}

json toStopOrderJson(list<StopOrderDto>& stopOrders) {
    json jsonArray = json::array();

    if (stopOrders.empty()) {
        return jsonArray;
    }

    for (const auto& stopOrder : stopOrders) {
        Option<StopOrderDto> stopOrderOption(stopOrder);

        jsonArray.push_back(
            toStopOrderJson(stopOrderOption)
        );
    }
    return jsonArray;
}

json toStopOrderJson(Option<StopOrderDto>& stopOrderOption) {
    json jsonObject;

    if (stopOrderOption.isEmpty()) {
        return jsonObject;
    }
    StopOrderDto stopOrder = stopOrderOption.get();
    jsonObject["orderNum"] = stopOrder.orderNum;
    jsonObject["date"] = stopOrder.date;
    jsonObject["flags"] = stopOrder.flags;
    jsonObject["brokerRef"] = stopOrder.brokerRef;
    jsonObject["firmId"] = stopOrder.firmId;
    jsonObject["account"] = stopOrder.account;
    jsonObject["clientCode"] = stopOrder.clientCode;
    jsonObject["ticker"] = stopOrder.ticker;
    jsonObject["classCode"] = stopOrder.classCode;
    jsonObject["classType"] = stopOrder.classType;
    jsonObject["name"] = stopOrder.name;
    jsonObject["condition"] = stopOrder.condition;
    jsonObject["conditionType"] = stopOrder.conditionType;
    jsonObject["conditionPrice"] = stopOrder.conditionPrice;
    jsonObject["price"] = stopOrder.price;
    jsonObject["qty"] = stopOrder.qty;
    jsonObject["linkedOrder"] = stopOrder.linkedOrder;
    jsonObject["expiry"] = stopOrder.expiry;
    jsonObject["transId"] = stopOrder.transId;
    jsonObject["coOrderNum"] = stopOrder.coOrderNum;
    jsonObject["type"] = stopOrder.type;
    jsonObject["orderDate"] = stopOrder.orderDate;
    jsonObject["allTradeNum"] = stopOrder.allTradeNum;
    jsonObject["stopFlags"] = stopOrder.stopFlags;
    jsonObject["offset"] = stopOrder.offset;
    jsonObject["spread"] = stopOrder.spread;
    jsonObject["balance"] = stopOrder.balance;
    jsonObject["uid"] = stopOrder.uid;
    jsonObject["filledQty"] = stopOrder.filledQty;
    jsonObject["withdrawTime"] = stopOrder.withdrawTime;
    jsonObject["conditionPrice2"] = stopOrder.conditionPrice2;
    jsonObject["activeFromTime"] = stopOrder.activeFromTime;
    jsonObject["activeToTime"] = stopOrder.activeToTime;
    jsonObject["conditionTicker"] = stopOrder.conditionTicker;
    jsonObject["conditionClassCode"] = stopOrder.conditionClassCode;
    jsonObject["canceledUid"] = stopOrder.canceledUid;
    jsonObject["orderDateTime"] = stopOrder.orderDateTime;
    jsonObject["withdrawDate"] = stopOrder.withdrawDate;
    jsonObject["activationDate"] = stopOrder.activationDate;
    jsonObject["currency"] = stopOrder.currency;
    jsonObject["lotSize"] = stopOrder.lotSize;
    jsonObject["status"] = stopOrder.status;

    return jsonObject;
}
