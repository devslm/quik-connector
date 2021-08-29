//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKCANDLESERVICE_H
#define QUIK_CONNECTOR_QUIKCANDLESERVICE_H

#include <string>
#include <list>
#include <unordered_map>
#include <atomic>
#include <nlohmann/json.hpp>
#include "../utils/QuikUtils.h"
#include "../../lua/Lua.h"
#include "../../../dto/quik/connector/subscription/QuikSubscriptionDto.h"
#include "../../../dto/quik/candle/CandleDto.h"
#include "../../../dto/quik/connector/request/RequestDto.h"
#include "../../../mapper/quik/candle/CandleMapper.h"
#include "../../queue/QueueService.h"

using namespace std;
using namespace nlohmann;

class QueueService;
class Redis;

class QuikCandleService {
public:
    explicit QuikCandleService(QueueService *queueService);

    virtual ~QuikCandleService();

    void init();

    void destroy();

    bool isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState,
                            string& classCode,
                            string& ticker,
                            Interval& interval,
                            Option<UpdateCandleCallback>& updateCandleCallback);

    void unsubscribeFromAllCandles(lua_State *luaState);

    bool unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    Option<CandleDto> getLastCandle(lua_State *luaState, const CandlesRequestDto& candlesRequest);

    Option<CandleDto> getCandles(lua_State *luaState, const CandlesRequestDto& candlesRequest);

    Option<int> getCandlesSize(QuikSubscriptionDto *candleSubscription);

    bool isSubscribedToTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    bool subscribeToTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    bool unsubscribeFromTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

private:
    const string CANDLE_SUBSCRIPTION_CACHE_KEY = "candles:subscriptions";

    thread checkCandlesThread;
    unordered_map<string, QuikSubscriptionDto> candlesSubscriptions;
    recursive_mutex *mutexLock;
    atomic_bool isRunning;
    QueueService *queueService;

    void startCheckCandlesThread();

    void reloadSavedSubscriptions();

    void saveCandleSubscriptionToCache(string& classCode, string& ticker, Interval& interval);

    vector<cpp_redis::reply> QuikCandleService::loadSubscriptionsFromCache();

    string toSubscriptionCacheValue(string& classCode, string& ticker, Interval& interval);
};

#endif //QUIK_CONNECTOR_QUIKCANDLESERVICE_H