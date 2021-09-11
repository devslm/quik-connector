//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKCANDLEEMULATORSERVICE_H
#define QUIK_CONNECTOR_QUIKCANDLEEMULATORSERVICE_H

#include <string>
#include <list>
#include <unordered_map>
#include <atomic>
#include <nlohmann/json.hpp>
#include "../../utils/QuikUtils.h"
#include "../../../lua/Lua.h"
#include "../../../../dto/quik/connector/subscription/QuikSubscriptionDto.h"
#include "../../../../dto/quik/candle/CandleDto.h"
#include "../../../../dto/quik/connector/request/RequestDto.h"
#include "../../../../mapper/quik/candle/CandleMapper.h"
#include "../../../queue/QueueService.h"
#include "../../../../component/concurrent/SmallConcurrentMap.h"

class QuikCandleEmulatorService {
public:
    bool isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState,
                            string& classCode,
                            string& ticker,
                            Interval& interval,
                            Option<UpdateCandleCallback>& updateCandleCallback);

    void unsubscribeFromAllCandles(lua_State *luaState);

    bool unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);
};

#endif //QUIK_CONNECTOR_QUIKCANDLEEMULATORSERVICE_H
