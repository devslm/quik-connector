//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "OrderRepository.h"

void OrderRepository::saveAll(const list<OrderDto>& orders) {
    for (const auto& order : orders) {
        save(order);
    }
}

void OrderRepository::save(const OrderDto& order) {
    //Statement query(*db->getConnection(), "INSERT INTO " + ORDERS_TABLE_NAME + " VALUES ()");

    //query.exec();
}

list<OrderEntity> OrderRepository::getAll() {
    //Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME);
    list<OrderEntity> orders;

    //while (query.executeStep()) {
        //OrderEntity orderEntity = toOrderEntity(query);

        //orders.push_back(orderEntity);
    //}
    return orders;
}

Option<OrderEntity> OrderRepository::getById(uint64_t orderId) {
    /*Statement query(*db->getConnection(), "SELECT * FROM " + ORDERS_TABLE_NAME + " WHERE order_num = ?");
    //query.bind(0, Db::toSqlLiteBigInt(orderId));

    if (!query.executeStep()) {
        return {};
    }
    //OrderEntity orderEntity = toOrderEntity(query);
*/
    return {};
}
