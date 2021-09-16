//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikCandleService.h"

QuikCandleService::QuikCandleService(QueueService *queueService) {
    this->isRunning = true;
    this->mutexLock = luaGetMutex();
    this->queueService = queueService;
}

QuikCandleService::~QuikCandleService() {
    this->isRunning = false;

    reloadSubscriptionsThread.join();
    checkCandlesRequestsCompleteThread.join();
    checkUpdatedCandlesThread.join();
}

void QuikCandleService::init() {
    reloadSubscriptionsThread = thread([this]() {
        this_thread::sleep_for(chrono::seconds(1));

        reloadSavedSubscriptions();
    });
    checkCandlesRequestsCompleteThread = thread([this]() {
        this_thread::sleep_for(chrono::seconds(1));

        checkCandlesRequestsComplete();
    });
    checkUpdatedCandlesThread = thread([this]() {
        this_thread::sleep_for(chrono::seconds(1));

        handleUpdatedCandles();
    });
}

void QuikCandleService::destroy() {
    this->isRunning = false;
}

void QuikCandleService::reloadSavedSubscriptions() {
    try {
        while (isRunning && !redis->getConnection().is_connected()) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        auto subscriptions = loadSubscriptionsFromCache();

        if (subscriptions.empty()) {
            logger->info("Skipping reload candles subscriptions from cache because no subscriptions found....");
            return;
        }
        logger->info("Found: {} candles subscriptions in cache", subscriptions.size());

        for (const auto& subscriptionString : subscriptions) {
            auto subscription = json::parse(subscriptionString.as_string());
            auto classCode = subscription["classCode"].get<string>();
            auto ticker = subscription["ticker"].get<string>();
            auto interval = QuikUtils::getIntervalByName(subscription["interval"].get<string>());

            logger->info("Reload candle subscription with class code: {}, ticker: {} and interval: {}",
                classCode, ticker, subscription["interval"]);

            subscribeToCandles(luaGetState(), classCode, ticker, interval);
        }
    } catch (exception& exception) {
        logger->error("Could not reload candles subscriptions from cache! Reason: {}", exception.what());
    }
}

/**
 * Check candles requests to complete preparing.
 *
 * Candles prepared when datasource size > 0. Send data to queue when
 * prepared.
 */
void QuikCandleService::checkCandlesRequestsComplete() {
    int totalLoopsBeforePrintQueueSize = 50;
    SmallConcurrentMap<string, int> candlesRequestsTimeout;
    int loopTimeoutMillis = 200;
    int requestAwaitingTimeoutMillis = 20000;
    int requestAwaitingMaxLoops = (int)((double)requestAwaitingTimeoutMillis / (double)loopTimeoutMillis);

    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(loopTimeoutMillis));

        if (candlesRequests.isEmpty()) {
            continue;
        }

        for (auto &candlesRequest: candlesRequests.iterator()) {
            auto requestId = candlesRequest.key;
            auto candleSubscription = candlesRequest.value;

            try {
                auto candlesSize = getCandlesSize(&candleSubscription);
                auto intervalName = QuikUtils::getIntervalName(candleSubscription.interval);

                if (candlesSize.isEmpty()
                        || candlesSize.get() < 1) {
                    logger->debug("Candles not ready for class code: {}, ticker: {} and interval: {}, awaiting....",
                        candleSubscription.classCode, candleSubscription.ticker, intervalName);

                    auto currentRequestTimeout = candlesRequestsTimeout.get(requestId);

                    if (currentRequestTimeout.isPresent()) {
                        candlesRequestsTimeout.put(requestId, currentRequestTimeout.get() + 1);
                    } else {
                        candlesRequestsTimeout.put(requestId, 1);
                    }

                    if (currentRequestTimeout.isPresent()
                            && currentRequestTimeout.get() > requestAwaitingMaxLoops) {
                        logger->error(
                            "Could not get candles size for class code: {}, ticker: {} and interval: {}, because timeout!",
                            candleSubscription.classCode, candleSubscription.ticker, intervalName);

                        candlesRequestsTimeout.remove(requestId);
                        candlesRequests.remove(requestId);

                        ErrorResponseDto response(
                            requestId,
                            RESPONSE_CANDLES_TIMEOUT_ERROR,
                            "Could not get candles because timeout while waiting datasource size!"
                        );
                        queueService->pubSubPublish(QueueService::QUIK_CANDLES_TOPIC, response);
                    }
                    continue;
                }
                candleSubscription.dataSourceSize = candlesSize.get();
                Option<CandleDto> candleOption;
                CandleDto candle;

                mutexLock->lock();

                auto isSuccess = toCandleDto(&candleSubscription, &candle, 1, candleSubscription.dataSourceSize);

                mutexLock->unlock();

                if (isSuccess) {
                    candleOption = Option<CandleDto>(candle);
                } else {
                    logger->error("Could not get candle data with class code: {}, ticker: {} and interval: {}",
                        candleSubscription.classCode, candleSubscription.ticker, intervalName);
                    continue;
                }
                logger->debug("Candles prepared for class code: {}, ticker: {} and interval: {}. Total candles: {}",
                    candleSubscription.classCode, candleSubscription.ticker, intervalName, candlesSize.get());

                candlesRequestsTimeout.remove(requestId);
                candlesRequests.remove(requestId);

                SuccessResponseDto response(requestId, toCandleJson(candleOption));

                queueService->pubSubPublish(QueueService::QUIK_CANDLES_TOPIC, response);

                if (!candleSubscription.candlesReadyCallbacks.empty()) {
                    for (auto& candleReadyCallback : candleSubscription.candlesReadyCallbacks) {
                        candleReadyCallback(candleOption);
                    }
                }
            } catch (exception& exception) {
                auto errorMessage = string("Could not check candles request for complete! Reason: ").append(exception.what());

                logger->error(errorMessage);

                mutexLock->unlock();

                ErrorResponseDto response(requestId, RESPONSE_QUIK_LUA_ERROR, errorMessage);

                queueService->pubSubPublish(QueueService::QUIK_CANDLES_TOPIC, response);
            }
        }
        --totalLoopsBeforePrintQueueSize;

        if (totalLoopsBeforePrintQueueSize <= 0) {
            totalLoopsBeforePrintQueueSize = 0;

            logger->debug("Total candles requests waiting for complete in queue: {}", candlesRequests.getSize());
        }
    }
}

void QuikCandleService::handleUpdatedCandles() {
    while (isRunning) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (updatedCandles.isEmpty()) {
            continue;
        }

        for (auto& updatedCandle : updatedCandles.iterator()) {
            try {
                auto candleSubscriptionCacheKey = updatedCandle.key;
                auto updatedCandleIndex = updatedCandle.value;
                auto candlesSubscriptionOption = candlesSubscriptions.get(candleSubscriptionCacheKey);

                if (candlesSubscriptionOption.isEmpty()) {
                    logger->error(
                        "Could not send event with updated candle because can't get required subscription with key: {}!",
                        candleSubscriptionCacheKey);
                    return;
                }
                auto candleSubscription = candlesSubscriptionOption.get();
                auto intervalName = QuikUtils::getIntervalName(candleSubscription.interval);
                CandleDto candle;

                if (updatedCandleIndex < 1.0) {
                    logger->warn(
                        "Skipping handle updated candle with class code: {}, ticker: {} and interval: {} because index 0 is not valid (it should start from 1)",
                        candleSubscription.classCode, candleSubscription.ticker, intervalName);
                    return;
                }
                lock_guard<recursive_mutex> lockGuard(*mutexLock);

                if (!toCandleDto(&candleSubscription, &candle, (int) updatedCandleIndex, (int) updatedCandleIndex)) {
                    logger->error("Could not get updated candle data with class code: {}, ticker: {} and interval: {}",
                        candleSubscription.classCode, candleSubscription.ticker, intervalName);
                    return;
                }
                Option<CandleDto> candleOption = Option<CandleDto>(candle);
                auto changedCandle = toChangedCandleDto(candleOption);

                updatedCandles.remove(candleSubscriptionCacheKey);

                for (const auto &callback: candleSubscription.updateCandleCallbacks) {
                    callback(changedCandle);
                }
                SuccessResponseDto response(toChangedCandleJson(changedCandle));

                queueService->pubSubPublish(QueueService::QUIK_CANDLE_CHANGE_TOPIC, response);
            } catch (exception &exception) {
                auto errorMessage = string("Could not handle updated candle in callback! Reason: ").append(exception.what());

                logger->error(errorMessage);

                ErrorResponseDto response(RESPONSE_QUIK_LUA_ERROR, errorMessage);

                queueService->pubSubPublish(QueueService::QUIK_CANDLES_TOPIC, response);
            }
        }
    }
}

bool QuikCandleService::subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    Option<UpdateCandleCallback> emptyCallback;

    return subscribeToCandles(luaState, classCode, ticker, interval, emptyCallback);
}

bool QuikCandleService::subscribeToCandles(lua_State *luaState,
                                           string& classCode,
                                           string& ticker,
                                           Interval& interval,
                                           Option<UpdateCandleCallback>& updateCandleCallback) {
    auto intervalName = QuikUtils::getIntervalName(interval);
    auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    if (isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        logger->info("Skipping subscribe to candles with ticker: {} and interval: {} because already subscribed...",
            ticker, intervalName);

        if (updateCandleCallback.isPresent()) {
            logger->info("Add user callback to exists candles subscription with ticker: {} and interval: {}", ticker, intervalName);

            auto existsCandleSubscriptionOption = candlesSubscriptions.get(candlesSubscriptionsKey);

            if (existsCandleSubscriptionOption.isPresent()) {
                existsCandleSubscriptionOption.get()
                    .updateCandleCallbacks
                    .push_back(updateCandleCallback.get());
            }
        }
        return true;
    }
    logger->info("Subscribe to candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaGetGlobal(luaState, "CreateDataSource")) {
        logger->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {} because could not get lua global field <<CreateDataSource>>",
            classCode, ticker, intervalName);
        return {};
    }
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    // Push LUA interval constant value onto stack
    if (!luaGetGlobal(luaState, intervalName)) {
        logger->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {} because could not get lua global field for interval <<{}>>",
            classCode, ticker, intervalName, intervalName);
        return {};
    }
    auto result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        logger->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
             classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    // Save reference to data source and remove from stack, so stack size - 1
    auto dataSourceIndex = luaSaveReference(luaState);

    QuikSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.dataSourceSize = -1;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    auto isSuccess = addUpdateCallbackToDataSource(luaState, candleSubscription);

    if (!isSuccess) {
        return false;
    } else if (updateCandleCallback.isPresent()) {
        logger->info("Add user callback to updated candles subscription with ticker: {} and interval: {}", ticker, intervalName);

        candleSubscription.updateCandleCallbacks.push_back(updateCandleCallback.get());
    }
    auto candlesSize = getCandlesSize(&candleSubscription);

    if (candlesSize.isPresent()) {
        candleSubscription.dataSourceSize = candlesSize.get();
    } else {
        logger->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {} because class code or ticker may be invalid (data source is undefined)",
            classCode, ticker, intervalName);
        return false;
    }
    // Subscribe also to quotes otherwise we don't receive any events
    subscribeToTickerQuotes(luaState, classCode, ticker);

    candlesSubscriptions.put(candlesSubscriptionsKey, candleSubscription);

    saveCandleSubscriptionToCache(classCode, ticker, interval);

    logger->info("Subscribed to candles with class code: {}, ticker: {} and interval: {} (size: {})",
        classCode, ticker, intervalName, candleSubscription.dataSourceSize);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

bool QuikCandleService::addUpdateCallbackToDataSource(lua_State *luaState, QuikSubscriptionDto& quikSubscription) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto dataSource = luaLoadReference(luaState, quikSubscription.dataSourceIndex);
    auto intervalName = QuikUtils::getIntervalName(quikSubscription.interval);

    if (!luaGetField(luaState, dataSource, "SetUpdateCallback")) {
        logger->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua field <<SetUpdateCallback>>",
            quikSubscription.classCode, quikSubscription.ticker, intervalName);
        return false;
    }
    auto callback = [](lua_State* state) -> int {
        lock_guard<recursive_mutex> lockGuard(*luaGetMutex());

        try {
            auto quikCandleService = (QuikCandleService*) lua_topointer(state, lua_upvalueindex(1));
            auto updatedCandlesMap = (SmallConcurrentMap<string, int>*) lua_topointer(state, lua_upvalueindex(2));
            auto candleSubscriptionCacheKey = (string*) lua_topointer(state, lua_upvalueindex(3));

            if (quikCandleService == nullptr
                    || !quikCandleService->isRunning
                    || updatedCandlesMap == nullptr
                    || candleSubscriptionCacheKey == nullptr) {
                return 0;
            }
            double updatedCandleIndex = 0.0;

            if (!luaGetNumber(state, &updatedCandleIndex)) {
                logger->error("Could not send event with updated candle because can't get updated candle index! Reason: {}",
                    luaGetErrorMessage(state));
                return 0;
            }
            updatedCandlesMap->put(*candleSubscriptionCacheKey, (int)updatedCandleIndex);
        } catch (exception& exception) {
            logger->error("Could not handle updated candle in callback! Reason: {}", exception.what());
        }
        return 0;
    };
    lua_pushvalue(luaState, dataSource);
    lua_pushlightuserdata(luaState, this);
    lua_pushlightuserdata(luaState, &updatedCandles);
    lua_pushlightuserdata(luaState, new string(QuikUtils::createCandlesMapKey(quikSubscription.classCode, quikSubscription.ticker, intervalName)));
    lua_pushcclosure(luaState, callback, 3);

    auto result = lua_pcall(luaState, 2, 1, 0);

    if (LUA_OK != result) {
        logger->error("Could not set empty callback in get candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
            quikSubscription.classCode, quikSubscription.ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    auto isCallbackApplied = false;
    auto isSuccess = luaGetBoolean(luaState, &isCallbackApplied);

    if (!isSuccess || !isCallbackApplied) {
        logger->error("Could not get candles with class code: {}, ticker: {} and interval: {}! Reason: update callback not applied",
            quikSubscription.classCode, quikSubscription.ticker, intervalName);
        return false;
    }
    // Remove data source reference from the LUA stack
    lua_pop(luaState, 1);

    return true;
}

void QuikCandleService::unsubscribeFromAllCandles(lua_State *luaState) {
    logger->info("Unsubscribe from all candles");

    auto subscriptions = loadSubscriptionsFromCache();

    if (subscriptions.empty()) {
        logger->info("Skipping unsubscribe from all candles because no one subscriptions found....");
        return;
    }

    for (const auto& subscriptionString : subscriptions) {
        auto subscription = json::parse(subscriptionString.as_string());
        auto classCode = subscription["classCode"].get<string>();
        auto ticker = subscription["ticker"].get<string>();
        auto interval = QuikUtils::getIntervalByName(subscription["interval"].get<string>());

        unsubscribeFromCandles(luaState, classCode, ticker, interval);
    }
    logger->info("Successfully unsubscribed from: {} candles", subscriptions.size());
}

bool QuikCandleService::unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    auto intervalName = QuikUtils::getIntervalName(interval);

    if (!isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        logger->info("Skipping unsubscribe from candles with ticker: {} and interval: {} because already unsubscribed...",
            ticker, intervalName);
        return true;
    }
    logger->info("Unsubscribe from candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    /*auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);
    auto quikSubscription = candlesSubscriptions.find(candlesSubscriptionsKey)->second;
    // Remove subscription data from local cache from map before unsubscribe in QUIK
    candlesSubscriptions.erase(candlesSubscriptionsKey);

    // Give check updates thread some time to complete it work
    this_thread::sleep_for(chrono::milliseconds(100));

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto dataSource = luaLoadReference(luaState, quikSubscription.dataSourceIndex);

    // Tell QUIK to close candles data source
    if(!luaGetField(luaState, dataSource, "Close")) {
        logger->error("Could not close candles data source with class code: {}, ticker: {} and interval: {} because could not get lua global field <<Close>>",
            classCode, ticker, intervalName);
        return false;
    }
    lua_pushvalue(luaState, dataSource);
    auto result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        logger->error("Could not close candles data source with class code: {}, ticker: {} and interval: {}! Reason: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    auto isUnsubscribed = false;
    auto isSuccess = luaGetBoolean(luaState, &isUnsubscribed);

    if (!isSuccess || !isUnsubscribed) {
        logger->error("Could not unsubscribe from candles data source with class code: {}, ticker: {} and interval: {} because QUIK return false value!",
            classCode, ticker, intervalName);
        return false;
    }
    // Remove reference on data source from lua internal map
    luaRemoveReference(luaState, quikSubscription.dataSourceIndex);
    // Remove data source reference from the LUA stack
    lua_pop(luaState, 1);

    unsubscribeFromTickerQuotes(luaState, classCode, ticker);*/

    // Remove subscription data from REDIS
    vector<string> removeSubscriptions = {
        toSubscriptionCacheValue(classCode, ticker, interval)
    };
    redis->getConnection().srem(CANDLE_SUBSCRIPTION_CACHE_KEY, removeSubscriptions);
    redis->getConnection().sync_commit();

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

bool QuikCandleService::isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    auto intervalName = QuikUtils::getIntervalName(interval);
    auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    return candlesSubscriptions.containsKey(candlesSubscriptionsKey);
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
        logger->error("Could not load candles subscriptions from cache! Reason: {}", subscriptionsObject.error());
        return {};
    } else if (!subscriptionsObject.is_array()) {
        return {};
    }
    return subscriptionsObject.as_array();
}

bool QuikCandleService::getCandles(lua_State *luaState, const CandlesRequestDto& candlesRequest) {
    return getCandles(luaState, candlesRequest, Option<CandlesReadyCallback>());
}

bool QuikCandleService::getCandles(lua_State *luaState,
                                   const CandlesRequestDto& candlesRequest,
                                   Option<CandlesReadyCallback>& candlesReadyCallback) {
    auto intervalName = QuikUtils::getIntervalName(candlesRequest.interval);
    auto classCode = candlesRequest.classCode;
    auto ticker = candlesRequest.ticker;
    auto interval = candlesRequest.interval;

    if (candlesRequest.requestId == "") {
        logger->error("Could not get candles with class code: {}, ticker: {} and interval: {} because request id is required!",
            classCode, ticker, intervalName);
        return false;
    }
    QuikSubscriptionDto candleSubscription;

    logger->debug("Get candles with class code: {}, ticker: {} and interval: {}", classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    // If already subscribed we can get datasource from the cache otherwise we should create new datasource
    if (isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        auto candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);
        auto candleSubscriptionOption = candlesSubscriptions.get(candlesSubscriptionsKey);

        if (candleSubscriptionOption.isPresent()) {
            candleSubscription = candleSubscriptionOption.get();

            if (candlesReadyCallback.isPresent()) {
                logger->info("Add user callback to get candles request with ticker: {} and interval: {}", ticker, intervalName);

                candleSubscription.candlesReadyCallbacks.push_back(candlesReadyCallback.get());
            }
        } else {
            logger->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not find subscription data in exists subscriptions map!",
                classCode, ticker, intervalName);

            return false;
        }
    } else {
        if (!luaGetGlobal(luaState, "CreateDataSource")) {
            logger->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua global field <<CreateDataSource>>",
                classCode, ticker, intervalName);
            return false;
        }
        lua_pushstring(luaState, classCode.c_str());
        lua_pushstring(luaState, ticker.c_str());
        // Push LUA interval constant value onto stack
        if (!luaGetGlobal(luaState, intervalName)) {
            logger->error("Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua global field for interval <<{}>>",
                classCode, ticker, intervalName, intervalName);
            return false;
        }
        auto result = lua_pcall(luaState, 3 , 1, 0);

        if (LUA_OK != result) {
            logger->error("Could not get candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
                classCode, ticker, intervalName, luaGetErrorMessage(luaState));
            return false;
        }
        candleSubscription.luaState = luaState;
        candleSubscription.mutexLock = mutexLock;
        candleSubscription.dataSourceIndex = luaSaveReference(luaState);
        candleSubscription.dataSourceSize = -1;
        candleSubscription.classCode = classCode;
        candleSubscription.ticker = ticker;
        candleSubscription.interval = interval;

        if (candlesReadyCallback.isPresent()) {
            logger->info("Add user callback to get candles request with ticker: {} and interval: {}", ticker, intervalName);

            candleSubscription.candlesReadyCallbacks.push_back(candlesReadyCallback.get());
        }

        if (!requestNewCandlesDataFromServer(luaState, candlesRequest, candleSubscription)) {
            return false;
        }
    }
    // Postpone check when data source will be ready to separate thread
    candlesRequests.put(candlesRequest.requestId, candleSubscription);

    return true;
}

bool QuikCandleService::requestNewCandlesDataFromServer(lua_State *luaState,
                                                        const CandlesRequestDto& candlesRequest,
                                                        QuikSubscriptionDto& candleSubscription) {
    auto classCode = candleSubscription.classCode;
    auto ticker = candleSubscription.ticker;
    auto dataSource = luaLoadReference(luaState, candleSubscription.dataSourceIndex);
    auto interval = candleSubscription.interval;
    auto intervalName = QuikUtils::getIntervalName(interval);

    logger->debug("Candles not exists for class code: {}, ticker: {} and interval: {}. Requesting new data....",
        classCode, ticker, intervalName);

    // Tell QUIK to load and update candles data from server
    if (!luaGetField(luaState, dataSource, "SetEmptyCallback")) {
        logger->error(
            "Could not get candles with class code: {}, ticker: {} and interval: {} because could not get lua field <<SetEmptyCallback>>",
            classCode, ticker, intervalName
        );
        return false;
    }
    lua_pushvalue(luaState, dataSource);

    auto result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        logger->error(
            "Could not set empty callback in get candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    auto isEmptyCallbackApplied = false;
    auto isSuccess = luaGetBoolean(luaState, &isEmptyCallbackApplied);

    if (!isSuccess || !isEmptyCallbackApplied) {
        logger->error(
            "Could not get candles with class code: {}, ticker: {} and interval: {}! Reason: empty callback not applied",
            classCode, ticker, intervalName);
        return false;
    }
    // Remove data source reference and callback from the LUA stack
    lua_pop(luaState, 1);

    return true;
}

Option<int> QuikCandleService::getCandlesSize(QuikSubscriptionDto *candleSubscription) {
    Option<int> candlesSize;

    if (candleSubscription == nullptr) {
        logger->error("Could not get candles size because subscription data is empty!");
        return candlesSize;
    }
    auto intervalName = QuikUtils::getIntervalName(candleSubscription->interval);

    lock_guard<recursive_mutex> luaLockGuard(*mutexLock);

    try {
        lua_State *luaState = candleSubscription->luaState;
        auto dataSource = luaLoadReference(luaState, candleSubscription->dataSourceIndex);

        if (!luaGetField(luaState, dataSource, "Size")) {
            logger->error(
                "Could not get candles size with class code: {}, ticker: {} and interval: {} because could not get lua field <<Size>>",
                candleSubscription->classCode, candleSubscription->ticker, intervalName);
            return candlesSize;
        }
        lua_pushvalue(luaState, dataSource);
        auto result = lua_pcall(luaState, 1, 1, 0);

        if (LUA_OK != result) {
            logger->error("Could not get candles size with class code: {}, ticker: {} and interval: {}! Reason: {}",
                candleSubscription->classCode, candleSubscription->ticker, intervalName, luaGetErrorMessage(luaState));
            return candlesSize;
        }
        auto dataSourceSize = 0.0;

        if (luaGetNumber(luaState, &dataSourceSize)) {
            candlesSize = {(int) dataSourceSize};
        }
        // Remove loaded data source reference from the LUA stack
        lua_pop(luaState, 1);

        luaPrintStackSize(luaState, (string) __FUNCTION__);
    } catch (exception& exception) {
        logger->error("Could not get candles size for class code: {}, ticker: {} and interval: {}! Reason: {}",
            candleSubscription->classCode, candleSubscription->ticker, intervalName, exception.what());
    }
    return candlesSize;
}

bool QuikCandleService::isSubscribedToTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}};

    if (!luaCallFunction(luaState, IS_SUBSCRIBED_LEVEL_2_QUOTES_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", IS_SUBSCRIBED_LEVEL_2_QUOTES_FUNCTION_NAME);
        return {};
    }
    auto isSubscribed = false;

    if (!luaGetBoolean(luaState, &isSubscribed)) {
        logger->error("Could not check if subscribed to quotes with class code: {} and ticker: {}", classCode, ticker);
        return false;
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return isSubscribed;
}

bool QuikCandleService::subscribeToTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    if (isSubscribedToTickerQuotes(luaState, classCode, ticker)) {
        logger->info("Skipping subscribe to level 2 quotes with class code: {} and ticker: {} because already subscribed",
            classCode, ticker);
        return true;
    }
    logger->info("Subscribe to level 2 quotes with class code: {} and ticker: {}", classCode, ticker);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}};

    if (!luaCallFunction(luaState, SUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", SUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME);
        return {};
    }
    auto isSubscribed = false;

    if (!luaGetBoolean(luaState, &isSubscribed)) {
        logger->error("Could not subscribe to quotes with class code: {} and ticker: {}", classCode, ticker);
        return false;
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return isSubscribed;
}

bool QuikCandleService::unsubscribeFromTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    if (!isSubscribedToTickerQuotes(luaState, classCode, ticker)) {
        return true;
    }
    logger->info("Unsubscribe from level 2 quotes with class code: {} and ticker: {}", classCode, ticker);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}};

    if (!luaCallFunction(luaState, UNSUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", UNSUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME);
        return {};
    }
    auto isUnSubscribed = false;

    if (!luaGetBoolean(luaState, &isUnSubscribed)) {
        logger->error("Could not check unsubscribed from quotes with class code: {} and ticker: {}", classCode, ticker);
        return false;
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return isUnSubscribed;
}
