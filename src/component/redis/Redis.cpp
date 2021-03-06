//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "Redis.h"

Redis::Redis() {
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
}

Redis::~Redis() {
    if (!configService->getConfig().redis.isEnabled) {
        return;
    }
    logger->info("[Redis] Closed all connections");

    redisClient.disconnect(true);

    // According with the issue: https://github.com/cpp-redis/cpp_redis/issues/54
    tacopie::set_default_io_service({});

    WSACleanup();
}

void Redis::connect() {
    if (!configService->getConfig().redis.isEnabled) {
        return;
    }
    //! Windows netword DLL init
    WORD version = MAKEWORD(2, 2);
    WSADATA data;

    if (WSAStartup(version, &data) != 0) {
        logger->error("WSAStartup() failure");
    }
    redisClient.connect(
        configService->getConfig().redis.host,
        configService->getConfig().redis.port,
        [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status) {
            string message;

            switch (status) {
                case cpp_redis::client::connect_state::ok:
                    message = "Client connected to server";
                    break;
                case cpp_redis::client::connect_state::dropped:
                    message = "Client connection has dropped";
                    break;
                case cpp_redis::client::connect_state::failed:
                    message = "Client connect/reconnect failed";
                    break;
                case cpp_redis::client::connect_state::stopped:
                    message = "Client reconnect stopped";
                    break;
                default:
                    break;
            }

            if (!message.empty()) {
                logger->info("[Redis] {}", message);
            }
        }, 5000, 1000, redisReconnectAttempts
    );
    authenticate();

    redisClient.sync_commit();
}

void Redis::authenticate() {
    if (!configService->getConfig().redis.isEnabled
            || configService->getConfig().redis.password.isEmpty()) {
        return;
    }
    redisClient.auth(configService->getConfig().redis.password.get(), [](const cpp_redis::reply& reply) {
        if (reply.is_error()) {
            logger->error("[Redis] Could not authenticate client! Reason: {}", reply.as_string());
        } else {
            logger->info("[Redis] Client authenticated successfully");
        }
    });
}

cpp_redis::client& Redis::getConnection() {
    return redisClient;
}
