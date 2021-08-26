//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include "QuikCandleService.h"

QuikCandleService::QuikCandleService(QueueService *queueService) {
    this->isRunning = true;
    this->mutexLock = luaGetMutex();
    this->queueService = queueService;
}

QuikCandleService::~QuikCandleService() {
    this->isRunning = false;
}

void QuikCandleService::init() {
    checkCandlesThread = thread([this] {
        reloadSavedSubscriptions();
        startCheckCandlesThread();
    });
}

void QuikCandleService::destroy() {
    this->isRunning = false;

    checkCandlesThread.join();
}

void QuikCandleService::reloadSavedSubscriptions() {
    try {
        while (isRunning && !redis->getConnection().is_connected()) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        auto subscriptions = loadSubscriptionsFromCache();

        if (subscriptions.empty()) {
            LOGGER->info("Skipping reload candles subscriptions from cache because no subscriptions....");
            return;
        }
        LOGGER->info("Found: {} candles subscriptions in cache", subscriptions.size());

        for (const auto& subscriptionString : subscriptions) {
            auto subscription = json::parse(subscriptionString.as_string());
            auto classCode = subscription["classCode"].get<string>();
            auto ticker = subscription["ticker"].get<string>();
            auto interval = QuikUtils::getIntervalByName(subscription["interval"].get<string>());

            LOGGER->info("Reload candle subscription with class code: {}, ticker: {} and interval: {}",
                classCode, ticker, subscription["interval"]);

            subscribeToCandles(luaGetState(), classCode, ticker, interval);
        }
    } catch (exception& exception) {
        LOGGER->error("Could not reload candles subscriptions from cache! Reason: {}", exception.what());
    }
}

void QuikCandleService::startCheckCandlesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds (250));

        for (const auto& keyValue : candlesSubscriptions) {
            QuikSubscriptionDto candleSubscription = keyValue.second;

            // Get current data source size
            int candlesSize = 0;
            bool isSuccess = getCandlesSize(&candleSubscription, &candlesSize);

            if (isSuccess) {
                candleSubscription.dataSourceSize = candlesSize;
                CandleDto candle;
                Option<CandleDto> candleOption;

                if (candlesSize > 1) {
                    if (!toCandleDto(&candleSubscription, &candle, candleSubscription.dataSourceSize - 1, candleSubscription.dataSourceSize)) {
                        string intervalName = QuikUtils::getIntervalName(candleSubscription.interval);

                        LOGGER->error("Could not get updated candle data with class code: {}, ticker: {} and interval: {}",
                            candleSubscription.classCode, candleSubscription.ticker, intervalName);
                        continue;
                    }
                    candleOption = Option<CandleDto>(candle);
                }

                if (candleOption.isPresent()) {
                    Option<ChangedCandleDto> changedCandle = toChangedCandleDto(candleOption);

                    queueService->pubSubPublish(
                        QueueService::QUIK_CANDLE_CHANGE_TOPIC,
                        toChangedCandleJson(changedCandle).dump()
                    );
                }
            }
        }
    }
}

bool QuikCandleService::subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    string intervalName = QuikUtils::getIntervalName(interval);

    if (isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        LOGGER->info("Skipping subscribe to candles with ticker: {} and interval: {} because already subscribed...",
            ticker, intervalName);
        return true;
    }
    LOGGER->info("Subscribe to candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaGetGlobal(luaState, "CreateDataSource")) {
        LOGGER->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {} because could not get lua global field <<CreateDataSource>>",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return {};
    }
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    int result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {}",
             classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    // Wait while data will be loaded
    this_thread::sleep_for(chrono::milliseconds(300));

    int dataSourceIndex = luaSaveReference(luaState);

    QuikSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    getCandlesSize(&candleSubscription, &candleSubscription.dataSourceSize);

    string candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    candlesSubscriptions[candlesSubscriptionsKey] = candleSubscription;

    saveCandleSubscriptionToCache(classCode, ticker, interval);

    LOGGER->info("Subscribed to candles with class code: {}, ticker: {} and interval: {} (size: {})",
        classCode, ticker, intervalName, candleSubscription.dataSourceSize);

    return true;
}

bool QuikCandleService::unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    auto intervalName = QuikUtils::getIntervalName(interval);

    if (!isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        LOGGER->info("Skipping unsubscribe from candles with ticker: {} and interval: {} because already unsubscribed...",
            ticker, intervalName);
        return true;
    }
    LOGGER->info("Unsubscribe from candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);
    auto quikSubscription = candlesSubscriptions.find(candlesSubscriptionsKey)->second;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto dataSource = luaLoadReference(luaState, quikSubscription.dataSourceIndex);

    // Tell QUIK to close candles data source
    lua_getfield(luaState, dataSource, "Close");
    lua_pushvalue(luaState, dataSource);
    auto result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not close candles data source with class code: {}, ticker: {} and interval: {}! Reason: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    bool isUnsubscribed = false;
    auto isSuccess = luaGetBoolean(luaState, &isUnsubscribed);

    if (!isSuccess || !isUnsubscribed) {
        LOGGER->error("Could not unsubscribe from candles data source with class code: {}, ticker: {} and interval: {} because QUIK return false value!",
            classCode, ticker, intervalName);
        return false;
    }
    // Remove subscription data from local cache from map
    candlesSubscriptions.erase(candlesSubscriptionsKey);
    // Remove subscription data from REDIS
    vector<string> removeSubscriptions = {
        toSubscriptionCacheValue(classCode, ticker, interval)
    };
    redis->getConnection().srem(CANDLE_SUBSCRIPTION_CACHE_KEY, removeSubscriptions);
    redis->getConnection().sync_commit();
    // Remove reference on data source from lua internal map
    luaRemoveReference(luaState, quikSubscription.dataSourceIndex);

    return true;
}

bool QuikCandleService::isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    auto intervalName = QuikUtils::getIntervalName(interval);
    auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    return (candlesSubscriptions.find(candlesSubscriptionsKey) != candlesSubscriptions.end());
}

void QuikCandleService::saveCandleSubscriptionToCache(string& classCode, string& ticker, Interval& interval) {
    vector<string> subscriptions = {
        toSubscriptionCacheValue(classCode, ticker, interval)
    };
    redis->getConnection().sadd(CANDLE_SUBSCRIPTION_CACHE_KEY, subscriptions);
    redis->getConnection().sync_commit();
}

string QuikCandleService::toSubscriptionCacheValue(string& classCode, string& ticker, Interval& interval) {
    json jsonObject;
    jsonObject["classCode"] = classCode;
    jsonObject["ticker"] = ticker;
    jsonObject["interval"] = QuikUtils::getIntervalName(interval);

    return jsonObject.dump();
}

vector<cpp_redis::reply> QuikCandleService::loadSubscriptionsFromCache() {
    auto subscriptionsFuture = redis->getConnection().smembers(CANDLE_SUBSCRIPTION_CACHE_KEY);

    redis->getConnection().sync_commit();

    auto subscriptionsObject = subscriptionsFuture.get();

    if (subscriptionsObject.is_error()) {
        LOGGER->error("Could not load candles subscriptions from cache! Reason: {}", subscriptionsObject.error());
        return {};
    } else if (!subscriptionsObject.is_array()) {
        return {};
    }
    return subscriptionsObject.as_array();
}

Option<CandleDto> QuikCandleService::getLastCandle(lua_State *luaState, const CandlesRequestDto& candlesRequest) {
    return {};
}

Option<CandleDto> QuikCandleService::getCandles(lua_State *luaState, const CandlesRequestDto& candlesRequest) {
    string intervalName = QuikUtils::getIntervalName(candlesRequest.interval);
    string classCode = candlesRequest.classCode;
    string ticker = candlesRequest.ticker;
    Interval interval = candlesRequest.interval;

    LOGGER->debug("Get candles with class code: {}, ticker: {} and interval: {}", classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaGetGlobal(luaState, "CreateDataSource")) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua global field <<CreateDataSource>>",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return {};
    }
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    int result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {}",
             classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return {};
    }
    int dataSourceIndex = luaSaveReference(luaState);
    int dataSource = luaLoadReference(luaState, dataSourceIndex);

    // Tell QUIK to load and update candles data
    if (!luaGetField(luaState, dataSource, "SetEmptyCallback")) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua field <<SetEmptyCallback>>",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return {};
    }
    lua_pushvalue(luaState, dataSource);
    result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not set empty callback in get candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));

        return {};
    }
    bool isEmptyCallbackApplied = false;
    bool isSuccess = luaGetBoolean(luaState, &isEmptyCallbackApplied);

    if (!isSuccess || !isEmptyCallbackApplied) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {}! Reason: empty callback not applied",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        lua_pop(luaState, 1);

        return {};
    }
    lua_pop(luaState, 2);

    // Wait while data will be loaded
    this_thread::sleep_for(chrono::milliseconds(300));

    QuikSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    isSuccess = getCandlesSize(&candleSubscription, &candleSubscription.dataSourceSize);

    Option<CandleDto> candleOption;

    if (isSuccess && candleSubscription.dataSourceSize > 0) {
        CandleDto candle;

        if (toCandleDto(&candleSubscription, &candle, 1, candleSubscription.dataSourceSize)) {
            candleOption = Option<CandleDto>(candle);
        } else {
            LOGGER->error("Could not get candle data with class code: {}, ticker: {} and interval: {}",
                candleSubscription.classCode, candleSubscription.ticker, intervalName);
        }
    }
    return candleOption;
}

bool QuikCandleService::getCandlesSize(QuikSubscriptionDto *candleSubscription, int *buffer) {
    if (candleSubscription == nullptr) {
        LOGGER->error("Could not get candles size because subscription data is empty!");
        return false;
    }
    lua_State *luaState = candleSubscription->luaState;
    string intervalName = QuikUtils::getIntervalName(candleSubscription->interval);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    int dataSource = luaLoadReference(luaState, candleSubscription->dataSourceIndex);

    if(!luaGetField(luaState, dataSource, "Size")) {
        LOGGER->error("Could not get candles size with class code: {}, ticker: {} and interval: {} because could not get lua field <<Size>>",
            candleSubscription->classCode, candleSubscription->ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    lua_pushvalue(luaState, dataSource);
    int result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not get candles size with class code: {}, ticker: {} and interval: {}! Reason: {}",
            candleSubscription->classCode, candleSubscription->ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    double dataSourceSize = 0.0;
    bool isSuccess = luaGetNumber(luaState, &dataSourceSize);

    if (isSuccess) {
        *buffer = (int)dataSourceSize;
    }
    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}
