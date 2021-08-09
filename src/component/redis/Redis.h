//
// Created by Sergey on 21.07.2021.
//

#ifndef QUIK_CONNECTOR_REDIS_H
#define QUIK_CONNECTOR_REDIS_H

#include <string>
#include <cpp_redis/cpp_redis>
#include <nlohmann/json.hpp>
#include "../../service/log/Logger.h"
#include "../../service/config/ConfigService.h"

using namespace std;
using namespace nlohmann;

class ConfigService;

extern ConfigService* configService;
extern shared_ptr<spdlog::logger> LOGGER;

class Redis {
public:
    Redis();

    virtual ~Redis();

    void connect();

    cpp_redis::client& getConnection();

private:
    int32_t redisReconnectAttempts;
    cpp_redis::client redisClient;

    void authenticate();
};

#endif //QUIK_CONNECTOR_REDIS_H
