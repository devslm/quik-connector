//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "library.h"
#include <iostream>

shared_ptr<spdlog::logger> logger;
Quik* quik;
ConfigService *configService;
Redis* redis;
Db* db;

static Logger* loggerWrapper;
static ConfigDto config;
static thread userRobotThread;
static volatile bool isQuikStarted;

static void printQuikDisabledCallbacks();
static void printDbDisabledOptions();
static void printRedisDisabledOptions();

static int onStop(lua_State *luaState) {
    isQuikStarted = false;
    int returnCode = quik->onStop(luaState);

    userRobotThread.join();

    delete quik;
    delete db;
    delete redis;
    delete configService;
    delete loggerWrapper;

    logger.reset();

    return returnCode;
}

static int onAllTrade(lua_State *luaState) {
    if (isQuikStarted && config.quik.callback.onAllTradeEnabled) {
        return quik->onAllTrade(luaState);
    }
    return 0;
}

static int onQuote(lua_State *luaState) {
    if (isQuikStarted && config.quik.callback.onQuoteEnabled) {
        return quik->onQuote(luaState);
    }
    return 0;
}

static int onOrder(lua_State *luaState) {
    if (isQuikStarted && config.quik.callback.onOrderEnabled) {
        return quik->onOrder(luaState);
    }
    return 0;
}

static int onTransReply(lua_State *luaState) {
    if (isQuikStarted && config.quik.callback.onTransReplyEnabled) {
        return quik->onTransReply(luaState);
    }
    return 0;
}

static void initConfig(Option<string>& scriptPath) {
    configService = new ConfigService(scriptPath.get());
    config = configService->getConfig();
}

static void initlogger() {
    logger = Logger::init(config);
}

static void initServices(lua_State *luaState) {
    redis = new Redis();
    redis->connect();

    luaInit(luaState);

    quik = new Quik();
    quik->onStart(luaState);

    db = new Db();

    #ifdef INCLUDE_QUIK_FUNCTIONS_TEST_CALL
    debugQuikFunctions(luaState);
    #endif

    printQuikDisabledCallbacks();
    printDbDisabledOptions();
    printRedisDisabledOptions();
}

static void printQuikDisabledCallbacks() {
    if (!config.quik.callback.onAllTradeEnabled) {
        logger->warn("Quik OnAllTrade callback disabled");
    }

    if (!config.quik.callback.onQuoteEnabled) {
        logger->warn("Quik OnQuote callback disabled");
    }

    if (!config.quik.callback.onOrderEnabled) {
        logger->warn("Quik OnOrder callback disabled");
    }

    if (!config.quik.callback.onTransReplyEnabled) {
        logger->warn("Quik OnTransReply callback disabled");
    }
}

static void printDbDisabledOptions() {
    if (!config.quik.order.saveToDb) {
        logger->warn("Quik save orders to DB disabled");
    }
}

static void printRedisDisabledOptions() {
    if (!configService->getConfig().redis.isEnabled) {
        logger->warn("Redis disabled - no events will send to queue");
    }
}

static int onStart(lua_State *luaState) {
    Option<string> scriptPath = luaGetScriptPath(luaState);

    if (scriptPath.isEmpty()) {
        throw runtime_error("Could not start connector because can't retrieve script path!");
    }
    initConfig(scriptPath);
    // Init logger before any other services otherwise we don't see any logs!
    initlogger();
    initServices(luaState);

    isQuikStarted = true;
    userRobotThread = thread([luaState]() {
        RobotService robotService(quik);
        robotService.run(luaState);
    });

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

    lua_pushcclosure(luaState, onTransReply, 0);
    lua_setglobal(luaState, "OnTransReply");

    return 1;
}
