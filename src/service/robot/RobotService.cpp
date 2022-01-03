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

    CandlesRequestDto candlesRequest;
    candlesRequest.requestId = connector::Uuid::createRandom();
    candlesRequest.classCode = "SPBFUT";
    candlesRequest.ticker = "RIH2";
    candlesRequest.interval = Interval::INTERVAL_M2;

    // Calculate EMA 9 after candles prepared
    auto onCandlesReadyCallback = [](Option<CandleDto> candleOption) {
        if (candleOption.isEmpty()) {
            logger->error("[Robot] Could not get candles");
            return;
        }
        logger->info("[Robot] Got candles with size: {}", candleOption.get().values.size());

        int candlesSize = candleOption.get().values.size();

        #ifdef TA_LIB

        TA_Real* closePrice = new TA_Real[candlesSize];
        TA_Real* out = new TA_Real[candlesSize];
        TA_Integer outBeg;
        TA_Integer outNbElement;

        int i = 0;

        for (auto& candle : candleOption.get().values) {
            closePrice[i++] = candle.close;
        }
        auto retCode = TA_MA(0, candlesSize - 1,
                         &closePrice[0],
                         9,TA_MAType_EMA,
                         &outBeg, &outNbElement, &out[0]);

        if (TA_SUCCESS != retCode) {
            logger->info("[Robot] Could not calculate EMA 9!");
        } else {
            int lastValueIndex = outNbElement - 1;

            logger->info("[Robot] EMA 9 values: {}, {}, {}", out[lastValueIndex], out[lastValueIndex - 1], out[lastValueIndex - 2]);
        }
        delete[] closePrice;
        delete[] out;

        #endif
    };
    auto isSuccess = quik->getCandles(luaState, candlesRequest, Option<CandlesReadyCallback>(onCandlesReadyCallback));

    if (!isSuccess) {
        logger->error("[Robot] Could not get candles with class code: {}, ticker: {} and interval: INTERVAL_M1",
            candlesRequest.classCode, candlesRequest.ticker);
    }
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
