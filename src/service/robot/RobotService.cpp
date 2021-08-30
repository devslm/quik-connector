//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "RobotService.h"

RobotService::RobotService(Quik* quik) {
    this->quik = quik;
}

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
    //quik->subscribeToCandles(luaState, classCode, ticker, interval, callback);

    t = thread(
        [this]() {
            int loops = 20;
            bool o = quik->isRunning();
            int y = 0;

            /*while (quik->isRunning()) {
                //this_thread::sleep_for(chrono::milliseconds(500));

                --loops;

                if (loops <= 0) {
                    loops = 20;

                    //logger->info("Server time: {}", quik->getServerTime(luaState).get());
                }
            }*/
        }
    );
}
