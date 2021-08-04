//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERREPOSITORY_H
#define QUIK_CONNECTOR_ORDERREPOSITORY_H

#include "../../component/db/Db.h"
#include "../../dto/order/OrderDto.h"

class Db;

extern Db* db;

class OrderRepository {
public:
    OrderRepository() = default;

    ~OrderRepository() = default;

    void saveAll(const list<OrderDto>& orders);

    void save(const OrderDto& order);

    list<OrderDto> getAll();

    Option<OrderDto> getById(uint64_t orderId);

private:
    const string ORDERS_TABLE_NAME = "orders";
};

#endif //QUIK_CONNECTOR_ORDERREPOSITORY_H
