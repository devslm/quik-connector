//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "OrderRepository.h"

static long long toSqlBigInt(uint64_t value) {
    return (long long)value;
}

void OrderRepository::saveAll(const list<OrderDto>& orders) {
    for (const auto& order : orders) {
        save(order);
    }
}

void OrderRepository::save(const OrderDto& order) {
    Statement query(*db->getConnection(), "INSERT INTO " + ORDERS_TABLE_NAME + " VALUES ()");

    query.exec();
}

list<OrderDto> OrderRepository::getAll() {
    Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME);
    list<OrderDto> orders;

    int columnPosition = 0;

    while (query.executeStep()) {
        OrderDto order;
        // Demonstrate how to get some typed column value
        order.orderNum = query.getColumn(columnPosition++).getInt64();
        order.flags = query.getColumn(columnPosition++);
        order.brokerRef = query.getColumn(columnPosition++).getString();

        orders.push_back(order);
    }
    return orders;
}

Option<OrderDto> OrderRepository::getById(uint64_t orderId) {
    Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME + " WHERE order_num = ?");
    query.bind(0, toSqlBigInt(orderId));

    if (!query.executeStep()) {
        return Option<OrderDto>();
    }
    OrderDto order;

    return Option<OrderDto>(order);
}
