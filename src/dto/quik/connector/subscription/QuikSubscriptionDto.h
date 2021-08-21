//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
#define QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H

#include <string>
#include <mutex>
#include "../../../../service/quik/utils/QuikUtils.h"

using namespace std;

typedef struct QuikSubscriptionDto {
    lua_State *luaState;
    recursive_mutex *mutexLock;
    int dataSourceIndex;
    int dataSourceSize;
    string classCode;
    string ticker;
    Interval interval;
} QuikSubscriptionDto;

#endif //QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
