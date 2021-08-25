//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERREPOSITORY_H
#define QUIK_CONNECTOR_ORDERREPOSITORY_H

#include "../../../component/db/Db.h"
#include "../../../mapper/quik/order/OrderMapper.h"
#include "../../../entity/quik/order/OrderEntity.h"

class Db;

extern Db* db;

using namespace std;

/**
 * All methods static because this functionality is optional and simplicity
 * and we don't need new instances for this class.
 */
class OrderRepository {
public:
    OrderRepository() = default;

    ~OrderRepository() = default;

    static void saveAll(list<OrderEntity>& orderEntities);

    static void save(OrderEntity& orderEntity);

    static list<OrderEntity> getAll();

    static Option<OrderEntity> getById(uint64_t orderId);

private:
    static const int ORDER_INSERT_TOTAL_COLUMNS = 53;
    static const string ORDERS_TABLE_NAME;
    static const string ORDER_INSERT_BIND_PATTERN;
};

#endif //QUIK_CONNECTOR_ORDERREPOSITORY_H
