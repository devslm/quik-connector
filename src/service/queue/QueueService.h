//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUEUESERVICE_H
#define QUIK_CONNECTOR_QUEUESERVICE_H

#include <string>
#include <list>
#include <nlohmann/json.hpp>
#include <cpp_redis/cpp_redis>
#include "../log/Logger.h"
#include "../../component/redis/Redis.h"
#include "../../mapper/quik/request/RequestMapper.h"
#include "../../dto/quik/order/OrderDto.h"
#include "../quik/Quik.h"

using namespace nlohmann;
using namespace std;

class Quik;
class Redis;

extern Redis* redis;

enum QueueResponseType {
    QUEUE,
    TOPIC
};

typedef struct CommandRequestDto {
    CommandRequestDto(string command, string commandId, json commandJsonData) {
        this->command = command;
        this->commandId = commandId;
        this->commandJsonData = commandJsonData;
    }
    string command;
    string commandId;
    json commandJsonData;
} CommandRequestDto;

typedef struct CommandResponseDto {
    CommandResponseDto(string channel, QueueResponseType type, string message) {
        this->channel = channel;
        this->type = type;
        this->message = message;
    }
    string channel;
    QueueResponseType type;
    string message;
} CommandResponseDto;

class QueueService {
public:
    static const string QUIK_COMMAND_TOPIC;
    static const string QUIK_TICKERS_TOPIC;
    static const string QUIK_TICKER_QUOTES_TOPIC;
    static const string QUIK_USER_TOPIC;
    static const string QUIK_STOP_ORDERS_TOPIC;
    static const string QUIK_CANDLES_TOPIC;
    static const string QUIK_ALL_TRADES_TOPIC;
    static const string QUIK_CANDLE_CHANGE_TOPIC;
    static const string QUIK_SERVER_INFO_TOPIC;

    static const string QUIK_ORDERS_QUEUE;

    static const string QUIK_GET_USER_INFO_COMMAND;
    static const string QUIK_GET_CANDLES_COMMAND;
    static const string QUIK_GET_ORDERS_COMMAND;
    static const string QUIK_GET_NEW_ORDERS_COMMAND;
    static const string QUIK_GET_STOP_ORDERS_COMMAND;
    static const string QUIK_GET_TICKERS_COMMAND;
    static const string SUBSCRIBE_TO_CANDLES_COMMAND;
    static const string UNSUBSCRIBE_FROM_CANDLES_COMMAND;

    QueueService(Quik *quik, string host, int port);

    virtual ~QueueService();

    void subscribe();

    void publish(const string& channel, const string& message);

    void pubSubPublish(const string& channel, const string& message);

    static bool addRequestIdToResponse(json& jsonData, const string& requestId);

private:
    Quik *quik;
    string redisHost;
    int redisPort;
    int32_t redisReconnectAttempts;
    cpp_redis::subscriber redisSubscriber;
    thread commandResponseHandlerThread;
    thread commandResponseChangedCandleHandlerThread;
    thread commandPublisherThread;
    queue<CommandRequestDto> requestQueue;
    queue<CommandResponseDto> responseQueue;
    queue<CommandResponseDto> changedCandlesResponseQueue;
    mutex requestQueueMutex;
    mutex responseQueueMutex;
    mutex changedCandlesResponseQueueMutex;
    bool isRunning;

    void authenticate();

    void subscribeToCommandQueue();

    void publishOrders(list<OrderDto>& orders);

    void startCheckRequestsThread();

    void startCheckResponsesThread();

    void startCheckChangedCandleResponsesThread();
};

#endif //QUIK_CONNECTOR_QUEUESERVICE_H
