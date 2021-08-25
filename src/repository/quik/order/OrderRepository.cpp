//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include "OrderRepository.h"

const string OrderRepository::ORDERS_TABLE_NAME = "orders";
const string OrderRepository::ORDER_INSERT_BIND_PATTERN = stringRepeatWithoutFirstNChars(", ?", ORDER_INSERT_TOTAL_COLUMNS, 2);

void OrderRepository::saveAll(list<OrderEntity>& orderEntities) {
    for (auto& orderEntity : orderEntities) {
        save(orderEntity);
    }
}

void OrderRepository::save(OrderEntity& orderEntity) {
    SQLite::Statement query(
        *db->getConnection(),
        "INSERT INTO " + ORDERS_TABLE_NAME + " VALUES (" + ORDER_INSERT_BIND_PATTERN + ")"
    );
    int bindPosition = 1;

    query.bind(bindPosition++, Db::toSqlLiteBigInt(orderEntity.orderNum));

    query.exec();
}

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
