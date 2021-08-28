//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikSubscriptionService.h"

QuikSubscriptionService::QuikSubscriptionService() {
    // Recreate all exists subscriptions
    this->isRunning = true;
    this->mutexLock = luaGetMutex();
}

QuikSubscriptionService::~QuikSubscriptionService() {

}

void QuikSubscriptionService::subscribeToImpersonalTransactions(lua_State *luaState, string& classCode, string& ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto interval = Interval::INTERVAL_TICK;
    auto subscription = createDataSource(luaState, classCode, ticker, interval);

    if (subscription.isEmpty()) {
        logger->error("Could not subscribe to impersonal transactions because can't create datasource!");
        return;
    }
}

void QuikSubscriptionService::subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto subscription = createDataSource(luaState, classCode, ticker, interval);

    if (subscription.isEmpty()) {
        logger->error("Could not subscribe to candles because can't create datasource!");
        return;
    }
}

Option<QuikSubscriptionDto> QuikSubscriptionService::createDataSource(lua_State *luaState,
                                                                        string& classCode,
                                                                        string& ticker,
                                                                        Interval& interval) {
    string intervalName = QuikUtils::getIntervalName(interval);

    //if (isSubscribedToCandles(luaState, classCode, ticker, interval)) {
    //    logger->info("Skipping subscribe with ticker: {} and interval: {} because already subscribed...", ticker, intervalName);
    //    return true;
    //}
    logger->info("Subscribe to candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    lua_getglobal(luaState, "CreateDataSource");
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    auto result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        logger->error("Could not subscribe to candles with class code: {}, ticker: {} and interval: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return Option<QuikSubscriptionDto>();
    }
    // Save reference to avoid GC remove
    auto dataSourceIndex = luaL_ref(luaState, LUA_REGISTRYINDEX);

    // Push reference onto the stack
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, dataSourceIndex);

    QuikSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    getCandlesSize(&candleSubscription, &candleSubscription.dataSourceSize);

    return Option<QuikSubscriptionDto>(candleSubscription);
}

bool QuikSubscriptionService::getCandlesSize(QuikSubscriptionDto *candleSubscription, int *buffer) {
    if (candleSubscription == nullptr) {
        logger->error("Could not get datasource size because subscription data is empty!");
        return false;
    }
    lua_State *luaState = candleSubscription->luaState;
    string intervalName = QuikUtils::getIntervalName(candleSubscription->interval);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    int dataSource = luaLoadReference(luaState, candleSubscription->dataSourceIndex);

    lua_getfield(luaState, dataSource, "Size");
    lua_pushvalue(luaState, dataSource);
    int result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        logger->error("Could not get candles size with class code: {}, ticker: {} and interval: {}! Reason: {}",
                      candleSubscription->classCode, candleSubscription->ticker, intervalName, luaGetErrorMessage(luaState));
        return false;
    }
    *buffer = (int)lua_tonumber(luaState, -1);

    lua_pop(luaState, 2);

    logger->debug("Stack size for <<getCandlesSize>> is: {}", lua_gettop(luaState));

    return true;
}

void QuikSubscriptionService::addSubscription(string& classCode,
                                              string& ticker,
                                              Interval& interval,
                                              bool& isImpersonalSubscription) {
    auto key = createSubscriptionKey(classCode, ticker, interval);

    if (isSubscriptionExists(classCode, ticker, interval, isImpersonalSubscription)) {
        return;
    }
    QuikSubscriptionDto quikSubscription;
    quikSubscription.classCode = classCode;
    quikSubscription.ticker = ticker;
    quikSubscription.interval = interval;

    if (isImpersonalSubscription) {
        impersonalTransactionSubscriptions[key] = quikSubscription;
    } else {
        candleSubscriptions[key] = quikSubscription;
    }
}

string QuikSubscriptionService::createSubscriptionKey(string& classCode, string& ticker, Interval& interval) {
    return classCode + "_" + ticker + "_" + QuikUtils::getIntervalName(interval);
}

bool QuikSubscriptionService::isSubscriptionExists(string& classCode,
                                                   string& ticker,
                                                   Interval& interval,
                                                   bool& isImpersonalSubscription) {
    auto key = createSubscriptionKey(classCode, ticker, interval);

    if (isImpersonalSubscription) {
        return (impersonalTransactionSubscriptions.find(key) == impersonalTransactionSubscriptions.end());
    } else {
        return (candleSubscriptions.find(key) == candleSubscriptions.end());
    }
}
