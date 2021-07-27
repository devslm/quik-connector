//
// Created by Sergey on 12.07.2021.
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
    static const string QUIK_CANDLES_QUEUE;
    static const string QUIK_TICKER_QUOTES_QUEUE;
    static const string QUIK_CANDLE_CHANGE_QUEUE;
    static const string QUIK_ORDERS_QUEUE;
    static const string QUIK_ALL_TRADES_QUEUE;

    static const string QUIK_GET_ORDERS_COMMAND;
    static const string QUIK_GET_NEW_ORDERS_COMMAND;

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
    list<CommandResponseDto> responseQueue;
    mutex responseQueueMutex;
    bool isRunning;

    void subscribeToCommandQueue();

    void publishOrders(list<OrderDto>& orders);

    bool addRequestIdToResponse(const string& commandId, json* jsonData);

    void startCheckResponsesThread();
};

#endif //QUIK_CONNECTOR_QUEUESERVICE_H
