//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
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

    Option<string> serverTime = quik->getServerTime(luaState);

    if (serverTime.isPresent()) {
        LOGGER->info("QUIK server time: {}", serverTime.get());
    } else {
        throw runtime_error("Could not debug QUIK server time function!");
    }
    Option<string> avgPingDuration = quik->getAvgPingDuration(luaState);

    if (avgPingDuration.isPresent()) {
        LOGGER->info("QUIK AVG ping: {}", avgPingDuration.get());
    } else {
        throw runtime_error("Could not debug QUIK avg ping function!");
    }
    Option<DepoLimitDto> depoLimit = quik->getDepoLimit(luaState, (string)"MC013***", (string)"152***", (string)"AGRO", (string)"L01-0*******", 2);

    if (depoLimit.isPresent()) {
        LOGGER->info("QUIK depo limit: {}", toDepoLimitJson(depoLimit).dump());
    } else {
        throw runtime_error("Could not debug QUIK depo limit function!");
    }
    string futureName = "RIU1";
    Option<double> stepCost = quik->getTickerPriceStepCost(luaState, "SPBFUT", futureName);

    if (stepCost.isPresent()) {
        LOGGER->info("Future: {} price step cost: {}", futureName, stepCost.get());
    } else {
        throw runtime_error("Could not debug QUIK price step cost function for: " + futureName);
    }
    set<string> clientCodes = quik->getClientCodes(luaState);

    if (clientCodes.empty()) {
        throw runtime_error("Could not debug QUIK get client codes function for because result is empty!");
    }

    for (const auto& clientCode : clientCodes) {
        LOGGER->info("Client code -> {}", clientCode);
    }
    Option<FutureLimitDto> futureLimit = quik->getFuturesLimit(luaState, (string)"MC013***", (string)"410***", 0, (string)"SUR");

    if (futureLimit.isPresent()) {
        LOGGER->info("Futures limits: {}", toFutureLimitJson(futureLimit).dump());
    } else {
        throw runtime_error("Could not debug QUIK get futures limits function for because result is empty!");
    }
}
