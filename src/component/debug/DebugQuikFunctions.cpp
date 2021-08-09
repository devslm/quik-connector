//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "DebugQuikFunctions.h"

void debugQuikFunctions(lua_State *luaState) {
    //quik->subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_M1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_M1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "BRQ1", Interval::INTERVAL_H1);

    CancelStopOrderRequestDto cancelStopOrderRequest;
    cancelStopOrderRequest.stopOrderId = QuikUtils::newTransactionId();
    cancelStopOrderRequest.account = "L01-00000F00";
    cancelStopOrderRequest.clientCode = "OPEN88380";
    cancelStopOrderRequest.classCode = "TQBR";
    cancelStopOrderRequest.ticker = "MAGN";

    //quik->cancelStopOrderById(luaState, cancelStopOrderRequest);

    LOGGER->info("Money limit: {}", toMoneyLimitJson(quik->getMoney(luaState, (string)"OPEN88380", (string)"L01-00000F00", (string)"EQTV", (string)"SUR")).dump());

    OrderRepository orderRepository;
}
