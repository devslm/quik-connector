//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERREPOSITORY_H
#define QUIK_CONNECTOR_ORDERREPOSITORY_H

#include "../../../component/db/Db.h"
#include "../../../mapper/quik/order/OrderMapper.h"
#include "../../../entity/quik/order/OrderEntity.h"

class Db;

extern Db* db;
extern shared_ptr<spdlog::logger> LOGGER;

using namespace std;

/**
 * All methods static because this functionality is optional and simplicity
 * and we don't need new instances for this class.
 */
class OrderRepository {
public:
    OrderRepository() = default;

    ~OrderRepository() = default;

    static list<OrderEntity> getAll();

    static Option<OrderEntity> getById(uint64_t orderId);

    static bool isExists(uint64_t orderId);

    static void upsertAll(list<OrderEntity>& orderEntities);

    static void upsert(OrderEntity& orderEntity);

    static void saveAll(list<OrderEntity>& orderEntities);

    static void save(OrderEntity& orderEntity);

    static void update(OrderEntity& orderEntity);

private:
    static const int ORDER_INSERT_TOTAL_COLUMNS = 53;
    static const string ORDERS_TABLE_NAME;
    static const string ORDER_INSERT_BIND_PATTERN;

    static int addBindsToAllFieldsWithoutId(SQLite::Statement& query, OrderEntity& orderEntity);

    static int addBindsToAllFields(SQLite::Statement& query, OrderEntity& orderEntity, bool isBindOrderId = true);
};

#endif //QUIK_CONNECTOR_ORDERREPOSITORY_H
