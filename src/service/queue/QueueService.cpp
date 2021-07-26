//
// Created by Sergey on 12.07.2021.
//

#include "QueueService.h"

const string QUIK_COMMAND_QUEUE = "queue:quik:commands";
const string QUIK_CONNECTION_STATUS_QUEUE = "queue:quik:connection:status";
const string QUIK_USER_QUEUE = "queue:quik:user";
const string QueueService::QUIK_CANDLES_QUEUE = "queue:quik:candles";
const string QUIK_LAST_CANDLE_QUEUE = "queue:quik:candles:last";
const string QueueService::QUIK_CANDLE_CHANGE_QUEUE = "queue:quik:candle:change";
const string QueueService::QUIK_TICKER_QUOTES_QUEUE = "queue:quik:ticker:quotes";
const string QueueService::QUIK_ORDERS_QUEUE = "queue:quik:orders";

const string QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND = "IS_QUIK_SERVER_CONNECTED";
const string QUIK_GET_USER_INFO_COMMAND = "GET_USER";
const string QUIK_GET_CANDLES_COMMAND = "GET_CANDLES";
const string QUIK_GET_LAST_CANDLE_COMMAND = "GET_LAST_CANDLE";
const string QueueService::QUIK_GET_ORDERS_COMMAND = "GET_ORDERS";
const string QueueService::QUIK_GET_NEW_ORDERS_COMMAND = "GET_NEW_ORDERS";

QueueService::QueueService(Quik *quik, string host, int port) {
    this->quik = quik;
    this->redisHost = host;
    this->redisPort = port;
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
    this->commandResponseHandlerThread = thread([this] {startCheckResponsesThread();});
    this->isRunning = true;
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
            continue;
        }
        responseQueueMutex.lock();

        CommandResponseDto commandResponse = responseQueue.front();
        responseQueue.pop_front();

        responseQueueMutex.unlock();

        try {
            if (QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND == commandResponse.command) {
                auto quikConnectionStatus = quik->getServerConnectionStatus(luaGetState());

                if (quikConnectionStatus.isPresent()) {
                    pubSubPublish(QUIK_CONNECTION_STATUS_QUEUE,toQuikServerConnectionStatusJson(&quikConnectionStatus));
                }
            } else if (QUIK_GET_USER_INFO_COMMAND == commandResponse.command) {
                auto quikUserInfo = quik->getUserName(luaGetState());

                if (quikUserInfo.isPresent()) {
                    pubSubPublish(QUIK_USER_QUEUE, toQuikUserInfoJson(&quikUserInfo));
                }
            } else if (QUIK_GET_ORDERS_COMMAND == commandResponse.command) {
                auto orders = quik->getOrders(luaGetState());

                publishOrders(orders);
            } else if (QUIK_GET_NEW_ORDERS_COMMAND == commandResponse.command) {
                auto newOrders = quik->getNewOrders(luaGetState());

                publishOrders(newOrders);
            } else if (QUIK_GET_LAST_CANDLE_COMMAND == commandResponse.command) {
                Option<LastCandleRequestDto> candlesRequest = toRequestDto<LastCandleRequestDto>(
                    commandResponse.commandJsonData);

                if (candlesRequest.isPresent()) {
                    auto lastCandleRequest = candlesRequest.get();
                    auto lastCandle = quik->getLastCandle(luaGetState(), &lastCandleRequest);
                    auto candleJson = toCandleJson(&lastCandle);

                    addRequestIdToResponse(commandResponse.commandId, &candleJson);

                    LOGGER->info("Candle JSON: {}", candleJson.dump());

                    if (true) {
                        pubSubPublish(QUIK_LAST_CANDLE_QUEUE, candleJson.dump());
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
    LOGGER->info("[Redis] Send orders to: {} with size: {}", QUIK_ORDERS_QUEUE, orders.size());

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
    redisSubscriber.subscribe(QUIK_COMMAND_QUEUE, [this](const std::string& channel, const std::string& message) {
        LOGGER->debug("[Redis] New incoming command: {}", message);

        json commandJsonData;
        auto isSuccess = parseCommandJson(message, &commandJsonData);
        auto commandId = commandJsonData["id"];
        auto commandName = commandJsonData["command"];

        if (!isSuccess || commandName.empty()) {
            LOGGER->info("[Redis] Could not handle incoming command: {} because JSON parse error!", message);
        } else {
            CommandResponseDto commandResponse(commandName, commandId, commandJsonData);

            responseQueueMutex.lock();

            responseQueue.push_back(commandResponse);

            responseQueueMutex.unlock();
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

bool QueueService::addRequestIdToResponse(const string& commandId, json *jsonData) {
    try {
        json requestIdJsonData;
        requestIdJsonData["requestId"] = commandId;

        jsonData->push_back(requestIdJsonData);

        LOGGER->info("JSON: %s", jsonData->dump().c_str());

        return true;
    } catch (json::parse_error& exception) {
        LOGGER->error("Could not add request id response json! Reason: {}!", exception.what());
    }
    return false;
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
