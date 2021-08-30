//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "RobotService.h"

void RobotService::run(lua_State* luaState) {
    // Implement robot custom logic here
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
}
