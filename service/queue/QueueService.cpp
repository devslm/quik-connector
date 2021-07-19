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

const string QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND = "IS_QUIK_SERVER_CONNECTED";
const string QUIK_GET_USER_INFO_COMMAND = "GET_USER";
const string QUIK_GET_CANDLES_COMMAND = "GET_CANDLES";
const string QUIK_GET_LAST_CANDLE_COMMAND = "GET_LAST_CANDLE";

QueueService::QueueService(Quik *quik, string host, int port) {
    this->quik = quik;
    this->redisHost = host;
    this->redisPort = port;
    this->redisReconnectAttempts = 1024 * 1024 * 1024;
}

QueueService::~QueueService() {
    logInfo("Queue service stopped");

    redisSubscriber.disconnect();
    redisClient.disconnect();
}

static bool parseCommandJson(const string& message, json* jsonData) {
    try {
        auto parsedData = json::parse(message);
        *jsonData = parsedData;

        return true;
    } catch (json::parse_error& exception) {
        logError("Could not parse queue command: %s to json object! Reason: %s!", message.c_str(), exception.what());
    }
    return false;
}

void QueueService::subscribe() {
    redisClient.connect(redisHost, redisPort, [](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
        string message;

        switch (status) {
            case cpp_redis::connect_state::ok:
                message = "Client connected to server";
                break;
            case cpp_redis::connect_state::dropped:
                message = "Client connection has dropped";
                break;
            case cpp_redis::connect_state::failed:
                message = "Client connect/reconnect failed";
                break;
            case cpp_redis::connect_state::stopped:
                message = "Client reconnect stopped";
                break;
            default:
                break;
        }

        if (!message.empty()) {
            logInfo("[Redis] %s", message.c_str());
        }
    }, 1000, redisReconnectAttempts);

    redisSubscriber.connect(redisHost, redisPort, [](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
        string message;

        switch (status) {
            case cpp_redis::connect_state::ok:
                message = "Subscriber connected to server";
                break;
            case cpp_redis::connect_state::dropped:
                message = "Subscriber connection has dropped";
                break;
            case cpp_redis::connect_state::failed:
                message = "Subscriber connect/reconnect failed";
                break;
            case cpp_redis::connect_state::stopped:
                message = "Subscriber reconnect stopped";
                break;
            default:
                break;
        }

        if (!message.empty()) {
            logInfo("[Redis] %s", message.c_str());
        }
    }, 1000, redisReconnectAttempts);

    redisSubscriber.subscribe(QUIK_COMMAND_QUEUE, [this](const std::string& channel, const std::string& message) {
        logDebug("[Redis] New incoming command: %s", message.c_str());

        json commandJsonData;
        bool isSuccess = parseCommandJson(message, &commandJsonData);
        string commandId = commandJsonData["id"];
        string commandName = commandJsonData["command"];

        if (!isSuccess || commandName.empty()) {
            logError("[Redis] Could not handle incoming command: %s because JSON parse error!", message.c_str());
        } else if (QUIK_IS_QUIK_SERVER_CONNECTED_COMMAND == commandName) {
            Option<QuikConnectionStatusDto> quikConnectionStatus = quik->getServerConnectionStatus(luaGetState());

            if (quikConnectionStatus.isPresent()) {
                pubSubPublish(QUIK_CONNECTION_STATUS_QUEUE, toQuikServerConnectionStatusJson(&quikConnectionStatus));
            }
        } else if (QUIK_GET_USER_INFO_COMMAND == commandName) {
            Option<QuikUserInfoDto> quikUserInfo = quik->getUserName(luaGetState());

            if (quikUserInfo.isPresent()) {
                pubSubPublish(QUIK_USER_QUEUE, toQuikUserInfoJson(&quikUserInfo));
            }
        } else if (QUIK_GET_LAST_CANDLE_COMMAND == commandName) {
            Option<LastCandleRequestDto> candlesRequest = toRequestDto<LastCandleRequestDto>(commandJsonData);

            if (candlesRequest.isPresent()) {
                auto lastCandleRequest = candlesRequest.get();
                auto lastCandle = quik->getLastCandle(luaGetState(), &lastCandleRequest);
                auto candleJson = toCandleJson(&lastCandle);

                addRequestIdToResponse(commandId, &candleJson);

                logInfo("Candle JSON: %s", candleJson.dump().c_str());

                if (true) {
                    pubSubPublish(QUIK_LAST_CANDLE_QUEUE, candleJson.dump());
                }
            }
        }
    });
    redisSubscriber.commit();
}

bool QueueService::addRequestIdToResponse(const string& commandId, json *jsonData) {
    try {
        json requestIdJsonData;
        requestIdJsonData["requestId"] = commandId;

        jsonData->push_back(requestIdJsonData);

        logInfo("JSON: %s", jsonData->dump().c_str());

        return true;
    } catch (json::parse_error& exception) {
        logError("Could not add request id response json! Reason: %s!", exception.what());
    }
    return false;
}

void QueueService::publish(const string& channel, const string& message) {
    logDebug("[Redis] Publish new message: %s to channel: %s", message.c_str(), channel.c_str());

    if (redisClient.is_connected()) {
        vector<string> values = { message };

        redisClient.rpush(channel, values);
        redisClient.commit();
    }
}

void QueueService::pubSubPublish(const string& channel, const string& message) {
    logDebug("[Redis] Publish pub/sub new message: %s to channel: %s", message.c_str(), channel.c_str());

    if (redisClient.is_connected()) {
        redisClient.publish(channel, message);
        redisClient.commit();
    }
}
