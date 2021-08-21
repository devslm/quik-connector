//
// Created by Sergey on 29.06.2021.
//

#ifndef QUIK_CONNECTOR_QUIKCANDLESERVICE_H
#define QUIK_CONNECTOR_QUIKCANDLESERVICE_H

#include <string>
#include <list>
#include <unordered_map>
#include <atomic>
#include "../utils/QuikUtils.h"
#include "../../lua/Lua.h"
#include "../../../dto/quik/connector/subscription/QuikSubscriptionDto.h"
#include "../../../dto/quik/candle/CandleDto.h"
#include "../../../dto/quik/connector/request/RequestDto.h"
#include "../../../mapper/quik/candle/CandleMapper.h"
#include "../../queue/QueueService.h"

using namespace std;

class QueueService;

extern shared_ptr<spdlog::logger> LOGGER;

class QuikCandleService {
public:
    QuikCandleService(QueueService *queueService);

    virtual ~QuikCandleService();

    void init();

    void destroy();

    bool isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool getCandlesSize(QuikSubscriptionDto *candleSubscription, int *buffer);

    Option<CandleDto> getLastCandle(lua_State *luaState, const CandlesRequestDto& candlesRequest);

    Option<CandleDto> getCandles(lua_State *luaState, const CandlesRequestDto& candlesRequest);

private:
    thread checkCandlesThread;
    unordered_map<string, QuikSubscriptionDto> candlesSubscriptions;
    recursive_mutex *mutexLock;
    atomic_bool isRunning;
    QueueService *queueService;

    void startCheckCandlesThread();
};

#endif //QUIK_CONNECTOR_QUIKCANDLESERVICE_H
