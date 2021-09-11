//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikCandleEmulatorService.h"

bool QuikCandleEmulatorService::isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return true;
}

bool subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return true;
}

bool QuikCandleEmulatorService::subscribeToCandles(lua_State *luaState,
                                                   string& classCode,
                                                   string& ticker,
                                                   Interval& interval,
                                                   Option<UpdateCandleCallback>& updateCandleCallback) {
    return true;
}

void QuikCandleEmulatorService::unsubscribeFromAllCandles(lua_State *luaState) {

}

bool QuikCandleEmulatorService::unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return true;
}
