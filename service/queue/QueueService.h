//
// Created by Sergey on 12.07.2021.
//

#ifndef QUIK_CONNECTOR_QUEUESERVICE_H
#define QUIK_CONNECTOR_QUEUESERVICE_H

#include <string>
#include <nlohmann/json.hpp>
#include <cpp_redis/cpp_redis>
#include "../../service/log/Logger.h"
#include "../../mapper/request/RequestMapper.h"
#include "../quik/Quik.h"

using namespace nlohmann;
using namespace std;

class Quik;

class QueueService {
public:
    static const string QUIK_CANDLES_QUEUE;
    static const string QUIK_TICKER_QUOTES_QUEUE;
    static const string QUIK_CANDLE_CHANGE_QUEUE;

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
    cpp_redis::client redisClient;
    cpp_redis::subscriber redisSubscriber;

    bool addRequestIdToResponse(const string& commandId, json* jsonData);
};

#endif //QUIK_CONNECTOR_QUEUESERVICE_H
