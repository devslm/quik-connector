//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUEUESERVICE_H
#define QUIK_CONNECTOR_QUEUESERVICE_H

#include <string>
#include <list>
#include <nlohmann/json.hpp>
#include <cpp_redis/cpp_redis>
#include "../log/Logger.h"
#include "../../component/redis/Redis.h"
#include "../../mapper/request/RequestMapper.h"
#include "../../dto/order/OrderDto.h"
#include "../quik/Quik.h"

using namespace nlohmann;
using namespace std;

class Quik;
class Redis;

extern Redis* redis;
extern shared_ptr<spdlog::logger> LOGGER;

typedef struct CommandResponseDto {
    CommandResponseDto(string command, string commandId, json commandJsonData) {
        this->command = command;
        this->commandId = commandId;
        this->commandJsonData = commandJsonData;
    }
    string command;
    string commandId;
    json commandJsonData;
} CommandResponseDto;

class QueueService {
public:
    static const string QUIK_COMMAND_TOPIC;
    static const string QUIK_CONNECTION_STATUS_TOPIC;
    static const string QUIK_TICKER_QUOTES_TOPIC;
    static const string QUIK_USER_TOPIC;
    static const string QUIK_STOP_ORDERS_TOPIC;
    static const string QUIK_CANDLES_TOPIC;
    static const string QUIK_LAST_CANDLE_TOPIC;
    static const string QUIK_ALL_TRADES_TOPIC;

    static const string QUIK_CANDLE_CHANGE_QUEUE;
    static const string QUIK_ORDERS_QUEUE;

    static const string QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND;
    static const string QUIK_GET_USER_INFO_COMMAND;
    static const string QUIK_GET_CANDLES_COMMAND;
    static const string QUIK_GET_LAST_CANDLE_COMMAND;
    static const string QUIK_GET_ORDERS_COMMAND;
    static const string QUIK_GET_NEW_ORDERS_COMMAND;
    static const string QUIK_GET_STOP_ORDERS_COMMAND;

    QueueService(Quik *quik, string host, int port);

    virtual ~QueueService();

    void subscribe();

    void publish(const string& channel, const string& message);

    void pubSubPublish(const string& channel, const string& message);

private:
    Quik *quik;
    string redisHost;
    int redisPort;
    int32_t redisReconnectAttempts;
    cpp_redis::subscriber redisSubscriber;
    thread commandResponseHandlerThread;
    queue<CommandResponseDto> responseQueue;
    bool isRunning;

    void authenticate();

    void subscribeToCommandQueue();

    void publishOrders(list<OrderDto>& orders);

    bool addRequestIdToResponse(json& jsonData, const string& requestId);

    void startCheckResponsesThread();
};

#endif //QUIK_CONNECTOR_QUEUESERVICE_H
