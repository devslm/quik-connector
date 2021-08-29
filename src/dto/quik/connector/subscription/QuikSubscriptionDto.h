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

typedef function<void(const Option<ChangedCandleDto>&)> UpdateCandleCallback;

typedef struct QuikSubscriptionDto {
    lua_State *luaState;
    recursive_mutex *mutexLock;
    int dataSourceIndex;
    int dataSourceSize;
    string classCode;
    string ticker;
    Interval interval;
    list<UpdateCandleCallback> callbacks;

} QuikSubscriptionDto;

#endif //QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
