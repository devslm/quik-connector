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
    //checkCandlesThread = thread([this] {startCheckCandlesThread();});
}

void QuikCandleService::destroy() {
    this->isRunning = false;

    //checkCandlesThread.join();
}

void QuikCandleService::startCheckCandlesThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::seconds(3));

        for (const auto& keyValue : candlesSubscriptions) {
            QuikSubscriptionDto candleSubscription = keyValue.second;

            int candlesSize = 0;
            bool isSuccess = getCandlesSize(&candleSubscription, &candlesSize);

            if (isSuccess) {
                CandleDto candle;

                if (candlesSize > 1) {
                    if (!toCandleDto(&candleSubscription, &candle, candlesSize - 1, candlesSize)) {
                        string intervalName = QuikUtils::getIntervalName(candleSubscription.interval);

                        LOGGER->error("Could not get candle data with class code: {}, ticker: {} and interval: {}",
                             candleSubscription.classCode, candleSubscription.ticker, intervalName);
                         continue;
                    }
                }
                Option<CandleDto> candleOption(candle);

                if (candleOption.isPresent()) {
                    Option<ChangedCandleDto> changedCandle = toChangedCandleDto(candleOption);

                    queueService->publish(QueueService::QUIK_CANDLE_CHANGE_QUEUE, toChangedCandleJson(changedCandle));
                }
            } else {
                LOGGER->error("No candles size!");
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
        LOGGER->info("Skipping subscribe to candles with ticker: {} because already subscribed...", ticker);
        return true;
    }
    LOGGER->info("Subscribe to candles with class code: {}, ticker: {} and interval: {}",
        classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    lua_getglobal(luaState, "CreateDataSource");
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
    this_thread::sleep_for(chrono::milliseconds(200));

    // Save reference to avoid GC remove
    int dataSourceIndex = luaL_ref(luaState, LUA_REGISTRYINDEX);
    // Push reference onto the stack
    //lua_rawgeti(luaState, LUA_REGISTRYINDEX, dataSourceIndex);

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

    LOGGER->info("Subscribed to candles with class code: {}, ticker: {} and interval: {} (size: {})",
        classCode, ticker, intervalName, candleSubscription.dataSourceSize);

    return true;
}

Option<CandleDto> QuikCandleService::getLastCandle(lua_State *luaState, const CandlesRequestDto& candlesRequest) {
    return Option<CandleDto>();
}

Option<CandleDto> QuikCandleService::getCandles(lua_State *luaState, const CandlesRequestDto& candlesRequest) {
    string intervalName = QuikUtils::getIntervalName(candlesRequest.interval);
    string classCode = candlesRequest.classCode;
    string ticker = candlesRequest.ticker;
    Interval interval = candlesRequest.interval;

    LOGGER->debug("Get candles with class code: {}, ticker: {} and interval: {}", classCode, ticker, intervalName);

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    list<CandleDto> candles;

    lua_getglobal(luaState, "CreateDataSource");
    lua_pushstring(luaState, classCode.c_str());
    lua_pushstring(luaState, ticker.c_str());
    lua_getglobal(luaState, intervalName.c_str());
    int result = lua_pcall(luaState, 3 , 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {}",
             classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        return Option<CandleDto>();
    }
    int dataSourceIndex = luaSaveReference(luaState);
    int dataSource = luaLoadReference(luaState, dataSourceIndex);

    // Tell QUIK to load and update candles data
    lua_getfield(luaState, dataSource, "SetEmptyCallback");
    lua_pushvalue(luaState, dataSource);
    result = lua_pcall(luaState, 1, 1, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not set empty callback in get candles with class code: {}, ticker: {} and interval: {}! Reason: {}",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));

        return Option<CandleDto>();
    }
    bool isEmptyCallbackApplied = false;
    bool isSuccess = luaGetBoolean(luaState, &isEmptyCallbackApplied);

    if (!isSuccess || !isEmptyCallbackApplied) {
        LOGGER->error("Could not get candles with class code: {}, ticker: {} and interval: {}! Reason: empty callback not applied",
            classCode, ticker, intervalName, luaGetErrorMessage(luaState));
        lua_pop(luaState, 1);

        return Option<CandleDto>();
    }
    lua_pop(luaState, 2);

    // Wait while data will be loaded
    this_thread::sleep_for(chrono::milliseconds(200));

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
    luaRemoveReference(luaState, dataSourceIndex);

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

    lua_getfield(luaState, dataSource, "Size");
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
