//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
#define QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H

#include <string>
#include <mutex>
#include "../../../../service/quik/utils/QuikUtils.h"
#include "../../candle/ChangedCandleDto.h"

using namespace std;

// Callbacks only for updated candles from setUpdateCallback in datasource
typedef function<void(Option<ChangedCandleDto>&)> UpdateCandleCallback;
// Callbacks only for ready candles with getCandles request
typedef function<void(Option<CandleDto>&)> CandlesReadyCallback;

typedef struct QuikSubscriptionDto {
    lua_State *luaState;
    recursive_mutex *mutexLock;
    int dataSourceIndex;
    int dataSourceSize;
    string classCode;
    string ticker;
    Interval interval;
    list<UpdateCandleCallback> updateCandleCallbacks;
    list<CandlesReadyCallback> candlesReadyCallbacks;
} QuikSubscriptionDto;

#endif //QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
