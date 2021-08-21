//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "OrderMapper.h"

static Option<TradeDto> getTradeByOrderId(uint64_t orderId, list<TradeDto> trades);

static void addOrderCommissionData(lua_State *luaState, Quik *quik, OrderDto* order);

bool toOrderDto(lua_State *luaState, Quik *quik, OrderDto* order) {
    if (!lua_istable(luaState, -1)) {
        LOGGER->error("Could not get table for order data! Current stack value type is: <<{}>> but required table!",
             luaGetType(luaState, -1));

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
        LOGGER->error("Could not get order class type with class code: {}", order->classCode);
        return false;
    }
    order->classType = classType.get();

    addOrderCommissionData(luaState, quik, order);

    Option<TickerDto> tickerOption = quik->getTickerById(luaState, order->classCode, order->ticker);

    if (tickerOption.isEmpty()) {
        LOGGER->error("Could not convert order data to dto! Reason: Can't get ticker data with class code: {} and ticker: {}",
            order->classCode, order->ticker);
        return false;
    }
    TickerDto ticker = tickerOption.get();
    order->currency = ticker.faceUnit;
    order->name = ticker.shortName;
    order->lotSize = ticker.lotSize;
    Option<double> priceStepCostOption = quik->getTickerPriceStepCost(luaState, order->classCode, order->ticker);

    if (priceStepCostOption.isEmpty()) {
        LOGGER->error("Could not convert order data to dto! Reason: Can't get ticker price step cost with class code: {} and ticker: {}",
            order->classCode, order->ticker);
        return false;
    }
    order->priceStepCost = priceStepCostOption.get();

    lua_pop(luaState, 2);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

static Option<TradeDto> getTradeByOrderId(uint64_t orderId, list<TradeDto> trades) {
    LOGGER->debug("Get trade data with order: {}", orderId);

    for (const auto& trade : trades) {
        if (trade.orderNum == orderId) {
            return Option<TradeDto>(trade);
        }
    }
    return Option<TradeDto>();
}

static void addOrderCommissionData(lua_State *luaState, Quik *quik, OrderDto* order) {
    if (configService->getConfig().quik.order.ignoreCancelled
            && order->status == ORDER_STATUS_CANCELED) {
        LOGGER->debug("Skipping add commission data to order: {} because it status: {}", order->orderNum, order->status);
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
        LOGGER->error("Could not get table for stop order data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

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
        LOGGER->error("Could not get stop order class type with class code: {}", stopOrder->classCode);
        return false;
    }
    stopOrder->classType = classType.get();

    Option<TickerDto> tickerOption = quik->getTickerById(luaState, stopOrder->classCode, stopOrder->ticker);

    if (!tickerOption.isPresent()) {
        LOGGER->error("Could not convert stop order data to dto! Reason: Can't get ticker data with class code: {} and ticker: {}",
            stopOrder->classCode, stopOrder->ticker);
        return false;
    }
    TickerDto ticker = tickerOption.get();
    stopOrder->currency = ticker.faceUnit;
    stopOrder->name = ticker.shortName;
    stopOrder->lotSize = ticker.lotSize;

    lua_pop(luaState, 2);

    return true;
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
