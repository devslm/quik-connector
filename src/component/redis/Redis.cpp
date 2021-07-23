//
// Created by Sergey on 21.07.2021.
//

#include "Redis.h"

Redis::Redis() {
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
}

Redis::~Redis() {
    logInfo("[Redis] Close all connections");

    redisClient.disconnect(true);

    // According with the issue: https://github.com/cpp-redis/cpp_redis/issues/54
    tacopie::set_default_io_service({});

    WSACleanup();
}

void Redis::connect() {
    //! Windows netword DLL init
    WORD version = MAKEWORD(2, 2);
    WSADATA data;

    if (WSAStartup(version, &data) != 0) {
        logError("WSAStartup() failure");
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
                logInfo("[Redis] %s", message.c_str());
            }
        }, 5000, 1000, redisReconnectAttempts
    );
    redisClient.sync_commit();
}

cpp_redis::client& Redis::getConnection() {
    return redisClient;
}
