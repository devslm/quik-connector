//
// Created by Sergey on 29.06.2021.
//

#ifndef QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H
#define QUIK_CONNECTOR_QUIKSUBSCRIPTIONDTO_H

#include <string>
#include <mutex>
#include "../../../service/quik/utils/QuikUtils.h"

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
