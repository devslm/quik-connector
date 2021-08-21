//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DEBUGQUIKFUNCTIONS_H
#define QUIK_CONNECTOR_DEBUGQUIKFUNCTIONS_H

#include "string"
#include "../../../service/quik/Quik.h"
#include "../../../service/log/Logger.h"
#include "../../../service/quik/utils/QuikUtils.h"
#include "../../../repository/quik/order/OrderRepository.h"

using namespace std;

class Quik;

extern Quik* quik;

void debugQuikFunctions(lua_State *luaState);

#endif //QUIK_CONNECTOR_DEBUGQUIKFUNCTIONS_H
