//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
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

    logger->info("Money limit: {}", toMoneyLimitJson(quik->getMoney(luaState, (string)"OPEN88380", (string)"L01-00000F00", (string)"EQTV", (string)"SUR")).dump());

    OrderRepository orderRepository;

    auto serverTime = quik->getServerTime(luaState);

    if (serverTime.isPresent()) {
        logger->info("QUIK server time: {}", serverTime.get());
    } else {
        throw runtime_error("Could not debug QUIK server time function!");
    }
    auto avgPingDuration = quik->getAvgPingDuration(luaState);

    if (avgPingDuration.isPresent()) {
        logger->info("QUIK AVG ping: {}", avgPingDuration.get());
    } else {
        throw runtime_error("Could not debug QUIK avg ping function!");
    }
    auto depoLimit = quik->getDepoLimit(luaState, (string)"MC013***", (string)"152***", (string)"AGRO", (string)"L01-0*******", 2);

    if (depoLimit.isPresent()) {
        logger->info("QUIK depo limit: {}", toDepoLimitJson(depoLimit).dump());
    } else {
        throw runtime_error("Could not debug QUIK depo limit function!");
    }
    string futureName = "RIU1";
    auto stepCost = quik->getTickerPriceStepCost(luaState, "SPBFUT", futureName);

    if (stepCost.isPresent()) {
        logger->info("Future: {} price step cost: {}", futureName, stepCost.get());
    } else {
        throw runtime_error("Could not debug QUIK price step cost function for: " + futureName);
    }
    auto clientCodes = quik->getClientCodes(luaState);

    if (clientCodes.empty()) {
        throw runtime_error("Could not debug QUIK get client codes function for because result is empty!");
    }

    for (const auto& clientCode : clientCodes) {
        logger->info("Client code -> {}", clientCode);
    }
    auto futureLimit = quik->getFuturesLimit(luaState, (string)"MC013***", (string)"410***", 0, (string)"SUR");

    if (futureLimit.isPresent()) {
        logger->info("Futures limits: {}", toFutureLimitJson(futureLimit).dump());
    } else {
        throw runtime_error("Could not debug QUIK get futures limits function for because result is empty!");
    }

    auto maxLots = quik->calcBuySell(luaState, (string)"SPBFUT", (string)"RIU1", (string)"MC013***", (string)"410***", 172500.0, true, false);

    if (futureLimit.isPresent()) {
        logger->info("Total BUY/SELL lots: {}", toMaxTradeLotsJson(maxLots).dump());
    } else {
        throw runtime_error("Could not debug QUIK get BUY/SELL total lots function for because result is empty!");
    }
}
