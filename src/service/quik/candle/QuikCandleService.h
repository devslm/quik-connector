//
// Created by Sergey on 29.06.2021.
//

#ifndef QUIK_CONNECTOR_QUIKCANDLESERVICE_H
#define QUIK_CONNECTOR_QUIKCANDLESERVICE_H

#include <string>
#include <unordered_map>
#include <atomic>
#include "../utils/QuikUtils.h"
#include "../../lua/Lua.h"
#include "../../../dto/candle/CandleSubscriptionDto.h"
#include "../../../dto/candle/CandleDto.h"
#include "../../../dto/connector/request/RequestDto.h"
#include "../../../mapper/candle/CandleMapper.h"
#include "../../queue/QueueService.h"

using namespace std;

class QueueService;

class QuikCandleService {
public:
    QuikCandleService(QueueService *queueService);

    virtual ~QuikCandleService();

    void init();

    void destroy();

    bool isSubscribedToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    bool subscribeToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    bool getCandlesSize(CandleSubscriptionDto *candleSubscription, int *buffer);

    Option<CandleDto> getLastCandle(lua_State *luaState, const LastCandleRequestDto *lastCandleRequest);

private:
    thread checkCandlesThread;
    unordered_map<string, CandleSubscriptionDto> candlesSubscriptions;
    recursive_mutex *mutexLock;
    atomic_bool isRunning;
    QueueService *queueService;

    void startCheckCandlesThread();
};

#endif //QUIK_CONNECTOR_QUIKCANDLESERVICE_H
