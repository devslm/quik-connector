#include "library.h"

#include <iostream>

Quik quik;
ConfigService *configService;

static int onStop(lua_State *luaState) {
    int returnCode = quik.onStop(luaState);

    quik.~Quik();

    delete configService;

    return returnCode;
}

static int onAllTrade(lua_State *luaState) {
    return quik.onAllTrade(luaState);
}

static int onQuote(lua_State *luaState) {
    return quik.onQuote(luaState);
}

static int onStart(lua_State *luaState) {
    string scriptPath;
    auto isSuccess = luaGetScriptPath(luaState, &scriptPath);

    if (!isSuccess) {
        return 1;
    }
    configService = new ConfigService(scriptPath);
    auto config = configService->getConfig();

    Logger logger(config);

    logInfo("Config log path: %s", config.scriptPath.c_str());

    luaInit(luaState);

    quik.onStart(luaState);

    /*quik.subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_D1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_M1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "BRQ1", Interval::INTERVAL_H1);*/

    while (quik.isRunning()) {
        quik.gcCollect(luaState);

        this_thread::sleep_for(chrono::milliseconds (800));
    }
    return 0;
}

// Library to be registered
static const struct luaL_Reg methods [] = {
    {nullptr, nullptr}
};

extern "C" __declspec(dllexport) int luaopen_quik_connector(lua_State *luaState) {
    luaL_newlib(luaState, methods);

    lua_pushcclosure(luaState, onStart, 0);
    lua_setglobal(luaState, "main");

    lua_pushcclosure(luaState, onStop, 0);
    lua_setglobal(luaState, "OnStop");

    lua_pushcclosure(luaState, onAllTrade, 0);
    lua_setglobal(luaState, "OnAllTrade");

    lua_pushcclosure(luaState, onQuote, 0);
    lua_setglobal(luaState, "OnQuote");

    return 1;
}
