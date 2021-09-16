//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QueueService.h"

const string QueueService::QUIK_COMMAND_TOPIC = "topic:quik:commands";
const string QueueService::QUIK_TICKERS_TOPIC = "topic:quik:tickers";
const string QueueService::QUIK_TICKER_QUOTES_TOPIC = "topic:quik:ticker:quotes";
const string QueueService::QUIK_USER_TOPIC = "topic:quik:user";
const string QueueService::QUIK_STOP_ORDERS_TOPIC = "topic:quik:stop:orders";
const string QueueService::QUIK_CANDLES_TOPIC = "topic:quik:candles";
const string QueueService::QUIK_ALL_TRADES_TOPIC = "topic:quik:trades:all";
const string QueueService::QUIK_TRADE_ACCOUNTS_TOPIC = "topic:quik:trade:accounts";
const string QueueService::QUIK_CANDLE_CHANGE_TOPIC = "topic:quik:candle:change";
const string QueueService::QUIK_SERVER_INFO_TOPIC = "topic:quik:server:info";
const string QueueService::QUIK_SUBSCRIPTION_RESULT_TOPIC = "topic:quik:subscription:result";
const string QueueService::QUIK_COMMON_ERROR_TOPIC = "topic:quik:errors";

const string QueueService::QUIK_ORDERS_QUEUE = "queue:quik:orders";

const string QueueService::QUIK_GET_USER_INFO_COMMAND = "GET_USER";
const string QueueService::QUIK_GET_CANDLES_COMMAND = "GET_CANDLES";
const string QueueService::QUIK_GET_ORDERS_COMMAND = "GET_ORDERS";
const string QueueService::QUIK_GET_NEW_ORDERS_COMMAND = "GET_NEW_ORDERS";
const string QueueService::QUIK_GET_STOP_ORDERS_COMMAND = "GET_STOP_ORDERS";
const string QueueService::QUIK_GET_TICKERS_COMMAND = "GET_TICKERS";
const string QueueService::QUIK_GET_TRADE_ACCOUNTS_COMMAND = "GET_TRADE_ACCOUNTS";
const string QueueService::SUBSCRIBE_TO_CANDLES_COMMAND = "SUBSCRIBE_TO_CANDLES";
const string QueueService::UNSUBSCRIBE_FROM_CANDLES_COMMAND = "UNSUBSCRIBE_FROM_CANDLES";

QueueService::QueueService(Quik *quik, string host, int port) {
    this->quik = quik;
    this->redisHost = host;
    this->redisPort = port;
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
    this->isRunning = true;

    if (configService->getConfig().redis.isEnabled) {
        this->commandResponseHandlerThread = thread([this] {startCheckRequestsThread();});
        this->commandPublisherThread = thread([this]() {startCheckResponsesThread();});
        this->commandResponseChangedCandleHandlerThread = thread([this]() {startCheckChangedCandleResponsesThread();});
    }
}

QueueService::~QueueService() {
    logger->info("[Redis] Queue service stopped");

    this->isRunning = false;

    if (!configService->getConfig().redis.isEnabled) {
        redisSubscriber.disconnect();
    }
    commandResponseHandlerThread.join();
    commandResponseChangedCandleHandlerThread.join();
    commandPublisherThread.join();
}

void QueueService::startCheckRequestsThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(5));

        if (requestQueue.empty()) {
            logger->trace("[Redis] Request queue is empty");
            continue;
        }
        requestQueueMutex.lock();
        CommandRequestDto commandRequest = requestQueue.front();
        requestQueueMutex.unlock();

        const string requestId = commandRequest.commandId;

        try {
            if (QUIK_GET_USER_INFO_COMMAND == commandRequest.command) {
                auto quikUserInfo = quik->getUserName(luaGetState());

                if (quikUserInfo.isPresent()) {
                    SuccessResponseDto response(requestId, toQuikUserInfoJson(&quikUserInfo));

                    pubSubPublish(QUIK_USER_TOPIC, response);
                } else {
                    ErrorResponseDto response(requestId, RESPONSE_QUIK_LUA_ERROR, RESPONSE_DEFAULT_ERROR_MESSAGE);

                    pubSubPublish(QUIK_USER_TOPIC, response);
                }
            } else if (QUIK_GET_ORDERS_COMMAND == commandRequest.command) {
                auto orders = quik->getOrders(luaGetState());

                publishOrders(orders);
            } else if (QUIK_GET_STOP_ORDERS_COMMAND == commandRequest.command) {
                auto stopOrders = quik->getStopOrders(luaGetState());
                SuccessResponseDto response(requestId, toStopOrderJson(stopOrders));

                pubSubPublish(QUIK_STOP_ORDERS_TOPIC, response);
            } else if (QUIK_GET_NEW_ORDERS_COMMAND == commandRequest.command) {
                auto newOrders = quik->getNewOrders(luaGetState());

                publishOrders(newOrders);
            } else if (QUIK_GET_TRADE_ACCOUNTS_COMMAND == commandRequest.command) {
                auto tradeAccounts = quik->getTradeAccounts(luaGetState());

                SuccessResponseDto response(requestId, toTradeAccountJson(tradeAccounts));

                pubSubPublish(QUIK_TRADE_ACCOUNTS_TOPIC, response);
            } else if (QUIK_GET_TICKERS_COMMAND == commandRequest.command) {
                auto request = toTickersRequestDto(commandRequest.commandJsonData);
                auto classCode = request.get().classCode;
                auto tickers = quik->getTickersByClassCode(luaGetState(), classCode);

                SuccessResponseDto response(requestId, toTickerJson(tickers));

                pubSubPublish(QUIK_TICKERS_TOPIC, response);
            } else if (QUIK_GET_CANDLES_COMMAND == commandRequest.command) {
                auto candlesRequestOption = toRequestDto<CandlesRequestDto>(commandRequest.commandJsonData);

                if (candlesRequestOption.isPresent()) {
                    auto candlesRequest = candlesRequestOption.get();
                    candlesRequest.requestId = requestId;
                    auto isSuccess = quik->getCandles(luaGetState(), candlesRequest);

                    if (!isSuccess) {
                        ErrorResponseDto response(requestId, RESPONSE_QUIK_LUA_ERROR, RESPONSE_DEFAULT_ERROR_MESSAGE);

                        pubSubPublish(QUIK_CANDLES_TOPIC, response);
                    }
                } else {
                    ErrorResponseDto response(
                        requestId,
                        RESPONSE_COMMAND_REQUEST_ERROR,
                        "Could not get candles because request is not valid!"
                    );
                    pubSubPublish(QUIK_CANDLES_TOPIC, response);
                }
            } else if (SUBSCRIBE_TO_CANDLES_COMMAND == commandRequest.command) {
                auto requestOption = toCandlesSubscribeRequestDto(commandRequest.commandJsonData);

                if (requestOption.isPresent()) {
                    logger->info("[Redis] New request to subscribe candles with data: {}", commandRequest.commandJsonData.dump());

                    auto request = requestOption.get();
                    auto isSubscribed = quik->subscribeToCandles(
                        luaGetState(),
                        request.classCode,
                        request.ticker,
                        request.interval
                    );
                    json jsonObject;
                    jsonObject["isSubscribed"] = isSubscribed;
                    SuccessResponseDto response(requestId, jsonObject);

                    pubSubPublish(QUIK_SUBSCRIPTION_RESULT_TOPIC, response);
                } else {
                    logger->error("[Redis] Could not handle new request to subscribe candles with data: {} because request data is invalid!",
                        commandRequest.commandJsonData.dump());

                    ErrorResponseDto response(
                        requestId,
                        RESPONSE_COMMAND_REQUEST_ERROR,
                        "Could not subscribe to candles because request is not valid!"
                    );
                    pubSubPublish(QUIK_SUBSCRIPTION_RESULT_TOPIC, response);
                }
            } else if (UNSUBSCRIBE_FROM_CANDLES_COMMAND == commandRequest.command) {
                auto requestOption = toCandlesSubscribeRequestDto(commandRequest.commandJsonData);

                if (requestOption.isPresent()) {
                    logger->info("[Redis] New request to unsubscribe candles with data: {}", commandRequest.commandJsonData.dump());

                    auto request = requestOption.get();
                    auto isUnsubscribed = quik->unsubscribeFromCandles(
                        luaGetState(),
                        request.classCode,
                        request.ticker,
                        request.interval
                    );
                    json jsonObject;
                    jsonObject["isUnsubscribed"] = isUnsubscribed;
                    SuccessResponseDto response(requestId, jsonObject);

                    pubSubPublish(QUIK_SUBSCRIPTION_RESULT_TOPIC, response);
                } else {
                    logger->error("[Redis] Could not handle new request to unsubscribe candles with data: {} because request data is invalid!",
                        commandRequest.commandJsonData.dump());

                    ErrorResponseDto response(
                        requestId,
                        RESPONSE_COMMAND_REQUEST_ERROR,
                        "Could not unsubscribe from candles because request is not valid!"
                    );
                    pubSubPublish(QUIK_SUBSCRIPTION_RESULT_TOPIC, response);
                }
            }
        } catch (const exception& exception) {
            logger->error("[Redis] Could not send response for command: {} with id: {}! Reason: {}",
                commandRequest.command, commandRequest.commandId, exception.what());
        }
        requestQueueMutex.lock();
        requestQueue.pop();
        requestQueueMutex.unlock();
    }
}
void QueueService::startCheckResponsesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (responseQueue.empty()) {
            logger->trace("[Redis] Response queue is empty");
            continue;
        } else if (!redis->getConnection().is_connected()) {
            continue;
        }
        responseQueueMutex.lock();
        CommandResponseDto commandResponse = responseQueue.front();
        responseQueueMutex.unlock();

        if (commandResponse.type == QueueResponseType::QUEUE) {
            logger->debug("[Redis] Publish new message: {} to channel: {}", commandResponse.message, commandResponse.channel);

            vector<string> values = {commandResponse.message};

            redis->getConnection().rpush(commandResponse.channel, values);
        } else {
            logger->debug("[Redis] Publish pub/sub new message: {} to channel: {}", commandResponse.message, commandResponse.channel);

            redis->getConnection().publish(commandResponse.channel, commandResponse.message);
        }
        redis->getConnection().commit();

        responseQueueMutex.lock();
        responseQueue.pop();
        responseQueueMutex.unlock();
    }
}

void QueueService::startCheckChangedCandleResponsesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (changedCandlesResponseQueue.empty()) {
            logger->trace("[Redis] Changed candles response queue is empty");
            continue;
        } else if (!redis->getConnection().is_connected()) {
            continue;
        }
        changedCandlesResponseQueueMutex.lock();
        CommandResponseDto commandResponse = changedCandlesResponseQueue.front();
        changedCandlesResponseQueueMutex.unlock();

        logger->debug("[Redis] Publish pub/sub new message: {} to channel: {}", commandResponse.message, commandResponse.channel);

        redis->getConnection().publish(commandResponse.channel, commandResponse.message);
        redis->getConnection().commit();

        changedCandlesResponseQueueMutex.lock();
        changedCandlesResponseQueue.pop();
        changedCandlesResponseQueueMutex.unlock();
    }
}

void QueueService::publishOrders(list<OrderDto>& orders) {
    if (!configService->getConfig().redis.isEnabled
            || orders.empty()) {
        return;
    }
    logger->debug("[Redis] Send orders to: {} with size: {}", QUIK_ORDERS_QUEUE, orders.size());

    publish(QUIK_ORDERS_QUEUE, toOrderJson(orders).dump());
}

static bool parseCommandJson(const string& message, json* jsonData) {
    try {
        auto parsedData = json::parse(message);
        *jsonData = parsedData;

        return true;
    } catch (json::parse_error& exception) {
        logger->error("[Redis] Could not parse queue command: {} to json object! Reason: {}!", message, exception.what());
    }
    return false;
}

void QueueService::subscribeToCommandQueue() {
    if (!configService->getConfig().redis.isEnabled) {
        return;
    }
    redisSubscriber.subscribe(QUIK_COMMAND_TOPIC, [this](const std::string& channel, const std::string& message) {
        if (!isRunning) {
            return;
        }
        logger->debug("[Redis] New incoming command: {} in channel: {}", message, channel);

        json commandJsonData;
        auto isSuccess = parseCommandJson(message, &commandJsonData);

        if (!isSuccess) {
            logger->error("[Redis] Could not handle incoming command: {} because JSON parse error!", message);
        } else {
            auto commandId = commandJsonData["id"];
            auto commandName = commandJsonData["command"];

            logger->debug("[Redis] Send response for command: {} (request queue size: {} and response queue size: {})",
                commandName, requestQueue.size(), responseQueue.size());

            CommandRequestDto commandResponse(commandName, commandId, commandJsonData);

            requestQueueMutex.lock();
            requestQueue.push(commandResponse);
            requestQueueMutex.unlock();
        }
    });
    redisSubscriber.commit();
}

void QueueService::subscribe() {
    if (!configService->getConfig().redis.isEnabled) {
        return;
    }
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
                logger->info("[Redis] {}", message);
            }
        }, 5000, 1000, redisReconnectAttempts
    );
    redisSubscriber.commit();
}

void QueueService::authenticate() {
    if (!configService->getConfig().redis.isEnabled
            || configService->getConfig().redis.password.isEmpty()) {
        return;
    }
    redisSubscriber.auth(configService->getConfig().redis.password.get(), [](const cpp_redis::reply& reply) {
        if (reply.is_error()) {
            logger->error("[Redis] Could not authenticate subscriber! Reason: {}", reply.as_string());
        } else {
            logger->info("[Redis] Subscriber authenticated successfully");
        }
    });
}

void QueueService::publish(const string& channel, SuccessResponseDto& response) {
    publish(channel, toResponseJson(response).dump());
}

void QueueService::publish(const string& channel, ErrorResponseDto& response) {
    publish(channel, toResponseJson(response).dump());
}

void QueueService::publish(const string& channel, const string& message) {
    if (!isRunning
            || !configService->getConfig().redis.isEnabled) {
        return;
    } else if (redis->getConnection().is_connected()) {
        CommandResponseDto commandResponse(channel, QueueResponseType::QUEUE, message);

        responseQueueMutex.lock();
        responseQueue.push(commandResponse);
        responseQueueMutex.unlock();
    }
}

void QueueService::pubSubPublish(const string& channel, SuccessResponseDto& response) {
    pubSubPublish(channel, toResponseJson(response).dump());
}

void QueueService::pubSubPublish(const string& channel, ErrorResponseDto& response) {
    pubSubPublish(channel, toResponseJson(response).dump());
}

void QueueService::pubSubPublish(const string& channel, const string& message) {
    if (!isRunning
            || !configService->getConfig().redis.isEnabled) {
        return;
    } else if (redis->getConnection().is_connected()) {
        CommandResponseDto commandResponse(channel, QueueResponseType::TOPIC, message);

        // Use separate thread for changed candles to avoid stuck on huge amount of
        // events when subscribe to new data source
        if (channel != QueueService::QUIK_CANDLE_CHANGE_TOPIC) {
            responseQueueMutex.lock();
            responseQueue.push(commandResponse);
            responseQueueMutex.unlock();
        } else {
            changedCandlesResponseQueueMutex.lock();
            changedCandlesResponseQueue.push(commandResponse);
            changedCandlesResponseQueueMutex.unlock();
        }
    }
}
