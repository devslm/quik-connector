//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "OrderRepository.h"

const string OrderRepository::ORDERS_TABLE_NAME = "orders";
const string OrderRepository::ORDER_INSERT_BIND_PATTERN = stringRepeatWithoutFirstNChars(", ?", ORDER_INSERT_TOTAL_COLUMNS, 2);

list<OrderEntity> OrderRepository::getAll() {
    SQLite::Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME);
    list<OrderEntity> orders;

    while (query.executeStep()) {
        OrderEntity orderEntity = toOrderEntity(query);

        orders.push_back(orderEntity);
    }
    return orders;
}

Option<OrderEntity> OrderRepository::getById(uint64_t orderId) {
    SQLite::Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME + " WHERE order_num = ?");
    query.bind(1, Db::toSqlLiteBigInt(orderId));

    if (!query.executeStep()) {
        return {};
    }
    OrderEntity orderEntity = toOrderEntity(query);

    return {orderEntity};
}

bool OrderRepository::isExists(uint64_t orderId) {
    Option<OrderEntity> orderEntity = getById(orderId);

    return orderEntity.isPresent();
}

void OrderRepository::upsertAll(list<OrderEntity>& orderEntities) {
    for (auto& orderEntity : orderEntities) {
        upsert(orderEntity);
    }
}

void OrderRepository::upsert(OrderEntity& orderEntity) {
    if (isExists(orderEntity.orderNum)) {
        update(orderEntity);
    } else {
        save(orderEntity);
    }
}

void OrderRepository::saveAll(list<OrderEntity>& orderEntities) {
    for (auto& orderEntity : orderEntities) {
        save(orderEntity);
    }
}

void OrderRepository::save(OrderEntity& orderEntity) {
    if (isExists(orderEntity.orderNum)) {
        LOGGER->info("Skipping save order: {} because already exists....", orderEntity.orderNum);
        return;
    }
    LOGGER->info("Save order: {} to DB", orderEntity.orderNum);

    SQLite::Statement query(
        *db->getConnection(),
        "INSERT INTO " + ORDERS_TABLE_NAME + " VALUES (" + ORDER_INSERT_BIND_PATTERN + ")"
    );
    addBindsToAllFields(query, orderEntity);

    query.exec();
}

void OrderRepository::update(OrderEntity& orderEntity) {
    if (!isExists(orderEntity.orderNum)) {
        LOGGER->info("Skipping update order: {} in DB because not found....", orderEntity.orderNum);
        return;
    }
    LOGGER->info("Update order: {} in DB", orderEntity.orderNum);

    SQLite::Statement query(
        *db->getConnection(),
        "UPDATE " + ORDERS_TABLE_NAME + " "
            "SET flags = ?, "
            "broker_ref = ?, "
            "user_id = ?, "
            "firm_id = ?, "
            "account = ?, "
            "price = ?, "
            "qty = ?, "
            "balance = ?, "
            "value = ?, "
            "accrued_int = ?, "
            "yield = ?, "
            "trans_id = ?, "
            "client_code = ?, "
            "price2 = ?, "
            "settle_code = ?, "
            "uid = ?, "
            "canceled_uid = ?, "
            "exchange_code = ?, "
            "activation_time = ?, "
            "linked_order = ?, "
            "expiry = ?, "
            "ticker = ?, "
            "name = ?, "
            "class_code = ?, "
            "class_type = ?, "
            "status = ?, "
            "type = ?, "
            "currency = ?, "
            "lot_size = ?, "
            "date = ?, "
            "withdraw_date = ?, "
            "bank_acc_id = ?, "
            "value_entry_type = ?, "
            "repo_term = ?, "
            "repo_value = ?, "
            "repo2value = ?, "
            "repo_value_balance = ?, "
            "start_discount = ?, "
            "reject_reason = ?, "
            "ext_order_flags = ?, "
            "min_qty = ?, "
            "exec_type = ?, "
            "side_qualifier = ?, "
            "acnt_type = ?, "
            "capacity = ?, "
            "passive_only_order = ?, "
            "visible = ?, "
            "price_step_cost = ?, "
            "commission_broker = ?, "
            "commission_clearing = ?, "
            "commission_tech_center = ?, "
            "commission_exchange = ? "
            "WHERE order_num = ?"
    );
    int bindPosition = addBindsToAllFieldsWithoutId(query, orderEntity);

    query.bind(bindPosition, Db::toSqlLiteBigInt(orderEntity.orderNum));

    query.exec();
}

int OrderRepository::addBindsToAllFieldsWithoutId(SQLite::Statement& query, OrderEntity& orderEntity) {
    return addBindsToAllFields(query, orderEntity, false);
}

int OrderRepository::addBindsToAllFields(SQLite::Statement& query, OrderEntity& orderEntity, bool isBindOrderId) {
    int bindPosition = 1;

    if (isBindOrderId) {
        query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.orderNum));
    }
    query.bind(bindPosition++, orderEntity.flags);
    query.bind(bindPosition++, orderEntity.brokerRef);
    query.bind(bindPosition++, orderEntity.userId);
    query.bind(bindPosition++, orderEntity.firmId);
    query.bind(bindPosition++, orderEntity.account);
    query.bind(bindPosition++, orderEntity.price);
    query.bind(bindPosition++, orderEntity.qty);
    query.bind(bindPosition++, orderEntity.balance);
    query.bind(bindPosition++, orderEntity.value);
    query.bind(bindPosition++, orderEntity.accruedInt);
    query.bind(bindPosition++, orderEntity.yield);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.transId));
    query.bind(bindPosition++, orderEntity.clientCode);
    query.bind(bindPosition++, orderEntity.price2);
    query.bind(bindPosition++, orderEntity.settleCode);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.uid));
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.canceledUid));
    query.bind(bindPosition++, orderEntity.exchangeCode);
    query.bind(bindPosition++, orderEntity.activationTime);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.linkedOrder));
    query.bind(bindPosition++, orderEntity.expiry);
    query.bind(bindPosition++, orderEntity.ticker);
    query.bind(bindPosition++, orderEntity.name);
    query.bind(bindPosition++, orderEntity.classCode);
    query.bind(bindPosition++, orderEntity.classType);
    query.bind(bindPosition++, orderEntity.status);
    query.bind(bindPosition++, orderEntity.type);
    query.bind(bindPosition++, orderEntity.currency);
    query.bind(bindPosition++, orderEntity.lotSize);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.date));
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.withdrawDate));
    query.bind(bindPosition++, orderEntity.bankAccId);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.valueEntryType));
    query.bind(bindPosition++, orderEntity.repoTerm);
    query.bind(bindPosition++, orderEntity.repoValue);
    query.bind(bindPosition++, orderEntity.repo2value);
    query.bind(bindPosition++, orderEntity.repoValueBalance);
    query.bind(bindPosition++, orderEntity.startDiscount);
    query.bind(bindPosition++, orderEntity.rejectReason);
    query.bind(bindPosition++, orderEntity.extOrderFlags);
    query.bind(bindPosition++, orderEntity.minQty);
    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.execType));
    query.bind(bindPosition++, orderEntity.sideQualifier);
    query.bind(bindPosition++, orderEntity.acntType);
    query.bind(bindPosition++, orderEntity.capacity);
    query.bind(bindPosition++, orderEntity.passiveOnlyOrder);
    query.bind(bindPosition++, orderEntity.visible);
    query.bind(bindPosition++, orderEntity.priceStepCost);
    query.bind(bindPosition++, orderEntity.commission.broker);
    query.bind(bindPosition++, orderEntity.commission.clearing);
    query.bind(bindPosition++, orderEntity.commission.techCenter);
    query.bind(bindPosition++, orderEntity.commission.exchange);

    return bindPosition;
}
