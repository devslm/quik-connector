//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "QueueService.h"

const string QueueService::QUIK_COMMAND_TOPIC = "topic:quik:commands";
const string QueueService::QUIK_CONNECTION_STATUS_TOPIC = "topic:quik:connection:status";
const string QueueService::QUIK_TICKER_QUOTES_TOPIC = "topic:quik:ticker:quotes";
const string QueueService::QUIK_USER_TOPIC = "topic:quik:user";
const string QueueService::QUIK_STOP_ORDERS_TOPIC = "topic:quik:stop:orders";
const string QueueService::QUIK_CANDLES_TOPIC = "topic:quik:candles";
const string QueueService::QUIK_LAST_CANDLE_TOPIC = "topic:quik:candles:last";
const string QueueService::QUIK_ALL_TRADES_TOPIC = "topic:quik:trades:all";

const string QueueService::QUIK_CANDLE_CHANGE_QUEUE = "queue:quik:candle:change";
const string QueueService::QUIK_ORDERS_QUEUE = "queue:quik:orders";

const string QueueService::QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND = "IS_QUIK_SERVER_CONNECTED";
const string QueueService::QUIK_GET_USER_INFO_COMMAND = "GET_USER";
const string QueueService::QUIK_GET_CANDLES_COMMAND = "GET_CANDLES";
const string QueueService::QUIK_GET_LAST_CANDLE_COMMAND = "GET_LAST_CANDLE";
const string QueueService::QUIK_GET_ORDERS_COMMAND = "GET_ORDERS";
const string QueueService::QUIK_GET_NEW_ORDERS_COMMAND = "GET_NEW_ORDERS";
const string QueueService::QUIK_GET_STOP_ORDERS_COMMAND = "GET_STOP_ORDERS";

QueueService::QueueService(Quik *quik, string host, int port) {
    this->quik = quik;
    this->redisHost = host;
    this->redisPort = port;
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
    this->isRunning = true;
    this->commandResponseHandlerThread = thread([this] {startCheckResponsesThread();});
}

QueueService::~QueueService() {
    LOGGER->info("Queue service stopped");

    this->isRunning = false;

    redisSubscriber.disconnect();
    commandResponseHandlerThread.join();
}

void QueueService::startCheckResponsesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (responseQueue.empty()) {
            LOGGER->debug("[Redis] Response queue is empty");
            continue;
        }
        CommandResponseDto commandResponse = responseQueue.front();
        responseQueue.pop();

        const string requestId = commandResponse.commandId;

        try {
            if (QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND == commandResponse.command) {
                auto quikConnectionStatus = quik->getServerConnectionStatus(luaGetState());

                if (quikConnectionStatus.isPresent()) {
                    pubSubPublish(
                        QUIK_CONNECTION_STATUS_TOPIC,
                        toQuikServerConnectionStatusJson(&quikConnectionStatus).dump()
                    );
                }
            } else if (QUIK_GET_USER_INFO_COMMAND == commandResponse.command) {
                auto quikUserInfo = quik->getUserName(luaGetState());

                if (quikUserInfo.isPresent()) {
                    pubSubPublish(QUIK_USER_TOPIC, toQuikUserInfoJson(&quikUserInfo).dump());
                }
            } else if (QUIK_GET_ORDERS_COMMAND == commandResponse.command) {
                auto orders = quik->getOrders(luaGetState());

                publishOrders(orders);
            } else if (QUIK_GET_STOP_ORDERS_COMMAND == commandResponse.command) {
                auto stopOrders = quik->getStopOrders(luaGetState());

                if (!stopOrders.empty()) {
                    pubSubPublish(QUIK_STOP_ORDERS_TOPIC, toStopOrderJson(stopOrders).dump());
                }
            } else if (QUIK_GET_NEW_ORDERS_COMMAND == commandResponse.command) {
                auto newOrders = quik->getNewOrders(luaGetState());

                publishOrders(newOrders);
            } else if (QUIK_GET_CANDLES_COMMAND == commandResponse.command) {
                auto candlesRequestOption = toRequestDto<CandlesRequestDto>(commandResponse.commandJsonData);

                if (candlesRequestOption.isPresent()) {
                    auto candlesRequest = candlesRequestOption.get();
                    auto candles = quik->getCandles(luaGetState(), candlesRequest);

                    if (candles.isPresent()) {
                        auto candlesJson = toCandleJson(candles);

                        addRequestIdToResponse(candlesJson, requestId);

                        pubSubPublish(QUIK_CANDLES_TOPIC, candlesJson.dump());
                    }
                }
            } else if (QUIK_GET_LAST_CANDLE_COMMAND == commandResponse.command) {
                auto candlesRequestOption = toRequestDto<CandlesRequestDto>(commandResponse.commandJsonData);

                if (candlesRequestOption.isPresent()) {
                    auto candlesRequest = candlesRequestOption.get();
                    auto lastCandle = quik->getLastCandle(luaGetState(), candlesRequest);
                    auto candleJson = toCandleJson(lastCandle);

                    LOGGER->info("Candle JSON: {}", candleJson.dump());

                    if (true) {
                        addRequestIdToResponse(candleJson, requestId);

                        pubSubPublish(QUIK_LAST_CANDLE_TOPIC, candleJson.dump());
                    }
                }
            }
        } catch (const exception& exception) {
            LOGGER->error("Could not send response for command: {} with id: {}! Reason: {}",
                commandResponse.command, commandResponse.commandId, exception.what());
        }
    }
}

void QueueService::publishOrders(list<OrderDto>& orders) {
    if (orders.empty()) {
        return;
    }
    LOGGER->debug("[Redis] Send orders to: {} with size: {}", QUIK_ORDERS_QUEUE, orders.size());

    publish(QUIK_ORDERS_QUEUE, toOrderJson(orders).dump());
}

static bool parseCommandJson(const string& message, json* jsonData) {
    try {
        auto parsedData = json::parse(message);
        *jsonData = parsedData;

        return true;
    } catch (json::parse_error& exception) {
        LOGGER->error("Could not parse queue command: {} to json object! Reason: {}!", message, exception.what());
    }
    return false;
}


void QueueService::subscribeToCommandQueue() {
    redisSubscriber.subscribe(QUIK_COMMAND_TOPIC, [this](const std::string& channel, const std::string& message) {
        LOGGER->debug("[Redis] New incoming command: {} in channel: {}", message, channel);

        json commandJsonData;
        auto isSuccess = parseCommandJson(message, &commandJsonData);
        auto commandId = commandJsonData["id"];
        auto commandName = commandJsonData["command"];

        if (!isSuccess || commandName.empty()) {
            LOGGER->error("[Redis] Could not handle incoming command: {} because JSON parse error!", message);
        } else {
            LOGGER->debug("[Redis] Send response for command: {} (response queue size: {})",
                 commandName, responseQueue.size());

            CommandResponseDto commandResponse(commandName, commandId, commandJsonData);

            responseQueue.push(commandResponse);
        }
    });
    redisSubscriber.commit();
}

void QueueService::subscribe() {
    redisSubscriber.connect(
        redisHost,
        redisPort,
        [this](const std::string& host, std::size_t port, cpp_redis::subscriber::connect_state status) {
            string message;

            switch (status) {
                case cpp_redis::subscriber::connect_state::ok:
                    authenticate();
                    subscribeToCommandQueue();
                    message = "Subscriber connected to server";
                    break;
                case cpp_redis::subscriber::connect_state::dropped:
                    message = "Subscriber connection has dropped";
                    break;
                case cpp_redis::subscriber::connect_state::failed:
                    message = "Subscriber connect/reconnect failed";
                    break;
                case cpp_redis::subscriber::connect_state::stopped:
                    message = "Subscriber reconnect stopped";
                    break;
                default:
                    break;
            }

            if (!message.empty()) {
                LOGGER->info("[Redis] {}", message);
            }
        }, 5000, 1000, redisReconnectAttempts
    );
    redisSubscriber.commit();
}

void QueueService::authenticate() {
    if (configService->getConfig().redis.password.isEmpty()) {
        return;
    }
    redisSubscriber.auth(configService->getConfig().redis.password.get(), [](const cpp_redis::reply& reply) {
        if (reply.is_error()) {
            LOGGER->error("[Redis] Could not authenticate subscriber! Reason: {}", reply.as_string());
        } else {
            LOGGER->info("[Redis] Subscriber authenticated successfully");
        }
    });
}

bool QueueService::addRequestIdToResponse(json& jsonData, const string& requestId) {
    if (jsonData == nullptr) {
        LOGGER->error("[Redis] Could not add request id to response data because json data is empty!");
        return false;
    } else if (requestId.empty()) {
        // Skip add request id if not present
        return true;
    }
    jsonData["requestId"] = requestId;

    return true;
}

void QueueService::publish(const string& channel, const string& message) {
    LOGGER->debug("[Redis] Publish new message: {} to channel: {}", message, channel);

    if (redis->getConnection().is_connected()) {
        vector<string> values = { message };

        redis->getConnection().rpush(channel, values);
        redis->getConnection().sync_commit();
    }
}

void QueueService::pubSubPublish(const string& channel, const string& message) {
    LOGGER->debug("[Redis] Publish pub/sub new message: {} to channel: {}", message, channel);

    if (redis->getConnection().is_connected()) {
        redis->getConnection().publish(channel, message);
        redis->getConnection().sync_commit();
    }
}
