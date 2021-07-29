#include "library.h"

#include <iostream>

shared_ptr<spdlog::logger> LOGGER;
Quik* quik;
Logger* logger;
ConfigService *configService;
Redis* redis;

static volatile bool isQuikStarted = false;

static int onStop(lua_State *luaState) {
    isQuikStarted = false;
    int returnCode = quik->onStop(luaState);

    delete quik;
    delete redis;
    delete configService;
    delete logger;

    LOGGER.reset();

    return returnCode;
}

static int onAllTrade(lua_State *luaState) {
    if (isQuikStarted) {
        return quik->onAllTrade(luaState);
    }
    return 0;
}

static int onQuote(lua_State *luaState) {
    if (isQuikStarted) {
        return quik->onQuote(luaState);
    }
    return 0;
}

static int onOrder(lua_State *luaState) {
    if (isQuikStarted) {
        return quik->onOrder(luaState);
    }
    return 0;
}

static int onStart(lua_State *luaState) {
    Option<string> scriptPath = luaGetScriptPath(luaState);

    if (scriptPath.isEmpty()) {
        throw runtime_error("Could not start connector because can't retrieve script path!");
    }
    configService = new ConfigService(scriptPath.get());
    auto config = configService->getConfig();
    // Init logger before any other services otherwise we don't see any logs!
    LOGGER = Logger::init(config);

    redis = new Redis();
    redis->connect();

    luaInit(luaState);

    quik = new Quik();
    quik->onStart(luaState);

    isQuikStarted = true;

    //quik->subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_M1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "RIU1", Interval::INTERVAL_M1);
    //quik.subscribeToCandles(luaState, "SPBFUT", "BRQ1", Interval::INTERVAL_H1);

    while (quik->isRunning()) {
        quik->gcCollect(luaState);

        this_thread::sleep_for(chrono::milliseconds(100));
    }
    isQuikStarted = false;

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

    lua_pushcclosure(luaState, onOrder, 0);
    lua_setglobal(luaState, "OnOrder");

    return 1;
}
