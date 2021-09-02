//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "RobotService.h"

RobotService::RobotService(Quik* quik) {
    this->quik = quik;
}

RobotService::~RobotService() {
    // Don't forget to join any internal threads otherwise DLL will crash
    robotInternalThread.join();
}

void RobotService::run(lua_State* luaState) {
    // Implement robot custom logic here
    // Current logic just for example
    runQuikConnectionStatusMonitor(luaState);

    string classCode = "SPBFUT";
    string ticker = "RIU1";
    auto interval = Interval::INTERVAL_M1;
    uint64_t lastBlastDate = 0;

    Option<UpdateCandleCallback> callback([&lastBlastDate](Option<ChangedCandleDto>& changedCandle) {
        if (changedCandle.isPresent()) {
            auto candle = changedCandle.get();
            auto previousCandleDiff = abs(candle.previousCandle.close - candle.previousCandle.open);

            if (lastBlastDate != candle.previousCandle.date
                    && previousCandleDiff < 1.0) {
                lastBlastDate = candle.previousCandle.date;

                logger->info("Found blast diff: {} in candle on time: {}", previousCandleDiff, candle.previousCandle.date);
            }
        }
    });
    quik->subscribeToCandles(luaState, classCode, ticker, interval, callback);

    logger->info("[Robot] Finished main thread");
}

void RobotService::runQuikConnectionStatusMonitor(lua_State* luaState) {
    robotInternalThread = thread([this, luaState]() {
        int totalLoops = 1200;

        while (quik->isRunning()) {
            // Avoid sleep for a long time because we can't exit properly due to QUIK timeout
            this_thread::sleep_for(chrono::milliseconds(100));

            --totalLoops;

            if (totalLoops >= 0) {
                continue;
            }
            totalLoops = 1200;
            auto connectionStatus = quik->getServerConnectionStatus(luaState);

            if (connectionStatus.isPresent()) {
                if (connectionStatus.get().isConnected) {
                    logger->info("[Robot] QUIK server time: {} (connection status: connected)",
                        quik->getServerTime(luaState).get());
                } else {
                    logger->info("[Robot] QUIK (connection status: disconnected)");
                }
            } else {
                logger->error("[Robot] Could not get QUIK connection information!");
            }
        }
    });
}
