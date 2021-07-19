//
// Created by Sergey on 29.06.2021.
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
    checkCandlesThread = thread([this] {startCheckCandlesThread();});
}

void QuikCandleService::destroy() {
    this->isRunning = false;

    checkCandlesThread.join();
}

void QuikCandleService::startCheckCandlesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::seconds(3));

        for (const auto& keyValue : candlesSubscriptions) {
            CandleSubscriptionDto candleSubscription = keyValue.second;

            int candlesSize = 0;
            bool isSuccess = getCandlesSize(&candleSubscription, &candlesSize);

            if (isSuccess) {
                CandleDto candle;

                if (candlesSize > 1) {
                    if (!toCandleDto(&candleSubscription, &candle, candlesSize - 1, candlesSize)) {
                        string intervalName = QuikUtils::getIntervalName(candleSubscription.interval);

                        logError("Could not get candle data with class code: %s, ticker: %s and interval: %s",
                             candleSubscription.classCode.c_str(), candleSubscription.ticker.c_str(), intervalName.c_str());
                         continue;
                    }
                }
                Option<CandleDto> candleOption(candle);

                if (candleOption.isPresent()) {
                    Option<ChangedCandleDto> changedCandle = toChangedCandleDto(&candleOption);

                    queueService->publish(
                        QueueService::QUIK_CANDLE_CHANGE_QUEUE,
                        toChangedCandleJson(&changedCandle).dump()
                    );
                }
            } else {
                logError("No candles size!");
            }
        }
    }
}

bool QuikCandleService::isSubscribedToCandles(lua_State *luaState, string classCode, string ticker, Interval interval) {
    string intervalName = QuikUtils::getIntervalName(interval);
    string candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    return (candlesSubscriptions.find(candlesSubscriptionsKey) != candlesSubscriptions.end());
}

bool QuikCandleService::subscribeToCandles(lua_State *luaState, string classCode, string ticker, Interval interval) {
    string intervalName = QuikUtils::getIntervalName(interval);

    if (isSubscribedToCandles(luaState, classCode, ticker, interval)) {
        logInfo("Skipping subscribe to candles with ticker: %s because already subscribed...", ticker.c_str());
        return true;
    }
    logInfo("Subscribe to candles with class code: %s, ticker: %s and interval: %s",
        classCode.c_str(), ticker.c_str(), intervalName.c_str());

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    lua_getglobal(luaState, "CreateDataSource");
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    int result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        logError("Could not subscribe to candles with class code: %s, ticker: %s and interval: %s",
                 classCode.c_str(), ticker.c_str(), intervalName.c_str(), luaGetErrorMessage(luaState));
        return false;
    }
    // Save reference to avoid GC remove
    int dataSourceIndex = luaL_ref(luaState, LUA_REGISTRYINDEX);
    // Push reference onto the stack
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, dataSourceIndex);

    CandleSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    getCandlesSize(&candleSubscription, &candleSubscription.dataSourceSize);

    string candlesSubscriptionsKey = QuikUtils::createCandlesMapKey(classCode, ticker, intervalName);

    candlesSubscriptions[candlesSubscriptionsKey] = candleSubscription;

    logInfo("Subscribed to candles with class code: %s, ticker: %s and interval: %s (size: %f)",
        classCode.c_str(), ticker.c_str(), intervalName.c_str(), candleSubscription.dataSourceSize);

    return true;
}

Option<CandleDto> QuikCandleService::getLastCandle(lua_State *luaState, const LastCandleRequestDto *lastCandleRequest) {
    string intervalName = QuikUtils::getIntervalName(lastCandleRequest->interval);
    string classCode = lastCandleRequest->classCode;
    string ticker = lastCandleRequest->ticker;
    Interval interval = lastCandleRequest->interval;

    logDebug("Get last candle with class code: %s, ticker: %s and interval: %s",
        classCode.c_str(), ticker.c_str(), intervalName.c_str());

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    lua_getglobal(luaState, "CreateDataSource");
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    int result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        logError("Could not get last candle with class code: %s, ticker: %s and interval: %s",
             classCode.c_str(), ticker.c_str(), intervalName.c_str(), luaGetErrorMessage(luaState));
        return Option<CandleDto>();
    }
    // Save reference to avoid GC remove
    int dataSourceIndex = luaL_ref(luaState, LUA_REGISTRYINDEX);
    // Push reference onto the stack
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, dataSourceIndex);

    CandleSubscriptionDto candleSubscription;
    candleSubscription.luaState = luaState;
    candleSubscription.mutexLock = mutexLock;
    candleSubscription.dataSourceIndex = dataSourceIndex;
    candleSubscription.classCode = classCode;
    candleSubscription.ticker = ticker;
    candleSubscription.interval = interval;

    bool isSuccess = getCandlesSize(&candleSubscription, &candleSubscription.dataSourceSize);

    if (isSuccess) {
        CandleDto candle;

        if (candleSubscription.dataSourceSize > 0) {
            /*if (!toCandleDto(&candleSubscription, &candle, candleSubscription.dataSourceSize)) {
                logError("Could not get last candle data with class code: %s, ticker: %s and interval: %s",
                     candleSubscription.classCode.c_str(), candleSubscription.ticker.c_str(), intervalName.c_str());
            } else {
                return Option<CandleDto>(candle);
            }*/
        }
    }
    return Option<CandleDto>();
}

bool QuikCandleService::getCandlesSize(CandleSubscriptionDto *candleSubscription, int *buffer) {
    if (candleSubscription == nullptr) {
        logError("Could not get candles size because subscription data is empty!");
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
        logError("Could not get candles size with class code: %s, ticker: %s and interval: %s! Reason: %s",
             candleSubscription->classCode.c_str(), candleSubscription->ticker.c_str(), intervalName.c_str(), luaGetErrorMessage(luaState));
        return false;
    }
    *buffer = (int)lua_tonumber(luaState, -1);

    lua_pop(luaState, 2);

    logDebug("Stack size for <<getCandlesSize>> is: %d", lua_gettop(luaState));

    return true;
}
