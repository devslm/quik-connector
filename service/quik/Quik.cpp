//
// Created by Sergey on 24.06.2021.
//

#include "Quik.h"

const char MESSAGE_FUNCTION_NAME[] = "message";
const char IS_CONNECTED_FUNCTION_NAME[] = "isConnected";
const char GET_CLASSES_LIST_FUNCTION_NAME[] = "getClassesList";
const char GET_CLASS_INFO_FUNCTION_NAME[] = "getClassInfo";
const char GET_INFO_PARAM_FUNCTION_NAME[] = "getInfoParam";
const char GET_QUOTE_LEVEL_2_FUNCTION_NAME[] = "getQuoteLevel2";

Quik::Quik() {
    this->mutexLock = luaGetMutex();
}

Quik::~Quik() {
    logInfo("Quik service destroyed");
}

bool Quik::isRunning() const {
    return isConnectorRunning;
}

int Quik::onStart(lua_State *luaState) {
    this->isConnectorRunning = true;
    string logMessage = APP_NAME + " started";
    auto config = configService->getConfig();

    //this->queueService = new QueueService(this, config.redis.host, config.redis.port);
    //this->queueService->subscribe();

    //this->quikCandleService = new QuikCandleService(this->queueService);
    //this->quikCandleService->init();

    this->checkQuotesThread = thread([this] {startCheckQuotesThread();});

    logInfo(logMessage.c_str());
    message(luaState, logMessage);

    return 0;
}

int Quik::onStop(lua_State *luaState) {
    isConnectorRunning = false;
    string logMessage = APP_NAME + " stopped";

    quikCandleService->destroy();

    checkQuotesThread.join();

    logInfo(logMessage.c_str());
    message(luaState, logMessage);

    return 0;
}

int Quik::onAllTrade(lua_State *L) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    allTradeLock.lock();

    TradeDto trade;
    bool isSuccess = toTradeDto(L, &trade);

    allTradeLock.unlock();

    if (isSuccess) {
        trades.push(trade);
    } else {
        // Send error
    }
    return 0;
}

Option<TradeDto> Quik::getNextTrade() {
    if (!trades.empty()) {
        Option<TradeDto> trade(trades.front());

        trades.pop();

        return trade;
    } else {
        Option<TradeDto> trade;

        return trade;
    }
}

int Quik::onQuote(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    quoteLock.lock();
    changedQuoteMapLock.lock();

    string classCode;
    string ticker;

    bool isSuccess = luaGetString(luaState, &classCode);

    if (!isSuccess) {
        logError("Could not get changed quote classCode");
        return 0;
    }
    isSuccess = luaGetString(luaState, &ticker);

    if (!isSuccess) {
        logError("Could not get changed quote ticker");
        return 0;
    }
    quoteLock.unlock();
    changedQuoteMapLock.unlock();

    changedQuotes[ticker] = classCode;

    return 0;
}

void Quik::startCheckQuotesThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(2));

        changedQuoteMapLock.lock();

        unordered_map<string, string> quotes = changedQuotes;

        changedQuoteMapLock.unlock();

        for (const auto& entry : quotes) {
            string ticker = entry.first;
            string classCode = entry.second;
            auto tickerQuotes = getTickerQuotes(luaGetState(), ticker, classCode);

            logInfo("New quotes: %s", toTickerQuoteJson(&tickerQuotes).dump().c_str());
        }
    }
}

void Quik::gcCollect(lua_State *luaState) {
    luaGcCollect(luaState);
}

void Quik::message(lua_State *luaState, string text) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{STRING_TYPE, text, 0, 0.0, false}};

    if (!luaCallFunction(luaState, MESSAGE_FUNCTION_NAME, 1, 0, args)) {
        logError("Could not call QUIK %s function!", MESSAGE_FUNCTION_NAME);
    } else {
        lua_pop(luaState, 1);
    }
}

Option<QuikConnectionStatusDto> Quik::getServerConnectionStatus(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaCallFunction(luaState, IS_CONNECTED_FUNCTION_NAME, 0, 1, nullptr)) {
        logError("Could not call QUIK %s function!", IS_CONNECTED_FUNCTION_NAME);
        return Option<QuikConnectionStatusDto>();
    }
    QuikConnectionStatusDto quikConnectionStatus;

    bool isSuccess = toQuikServerConnectionStatusDto(luaState, &quikConnectionStatus);

    if (isSuccess) {
        return Option<QuikConnectionStatusDto>(quikConnectionStatus);
    }
    return Option<QuikConnectionStatusDto>();
}

Option<QuikUserInfoDto> Quik::getUserName(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{STRING_TYPE, "USER", 0, 0.0, false}};

    if (!luaCallFunction(luaState, GET_INFO_PARAM_FUNCTION_NAME, 1, 1, args)) {
        logError("Could not call QUIK %s function to get user info!", GET_INFO_PARAM_FUNCTION_NAME);
        return Option<QuikUserInfoDto>();
    }
    QuikUserInfoDto quikUserInfo;
    bool isSuccess = toQuikUserInfoDto(luaState, &quikUserInfo);

    if (isSuccess) {
        return Option<QuikUserInfoDto>(quikUserInfo);
    }
    return Option<QuikUserInfoDto>();
}

set<string> Quik::getClassesList(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    set<string> classes;

    if (!luaCallFunction(luaState, GET_CLASSES_LIST_FUNCTION_NAME, 0, 1, nullptr)) {
        logError("Could not call QUIK %s function!", GET_CLASSES_LIST_FUNCTION_NAME);
        return classes;
    }
    const char *classesStr = lua_tostring(luaState, -1);
    lua_pop(luaState, 1);

    classes = stringSplitByDelimeter(classesStr, ',');

    return classes;
}

Option<ClassInfoDto> Quik::getClassInfo(lua_State *luaState, string *className) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{STRING_TYPE, *className, 0, 0.0, false}};

    if (!luaCallFunction(luaState, GET_CLASS_INFO_FUNCTION_NAME, 1, 1, args)) {
        logError("Could not call QUIK %s function!", GET_CLASS_INFO_FUNCTION_NAME);
        return Option<ClassInfoDto>();
    }
    ClassInfoDto classInfo;
    bool isSuccess = toClassInfoDto(luaState, &classInfo);

    if (isSuccess) {
        return Option<ClassInfoDto>(classInfo);
    }
    logError("Could not get class info for: %s!", className->c_str());

    return Option<ClassInfoDto>();
}

bool Quik::isSubscribedToCandles(lua_State *luaState, string classCode, string ticker, Interval interval) {
    return quikCandleService->isSubscribedToCandles(luaState, classCode, ticker, interval);
}

bool Quik::subscribeToCandles(lua_State *luaState, string classCode, string ticker, Interval interval) {
    return quikCandleService->subscribeToCandles(luaState, classCode, ticker, interval);
}

Option<CandleDto> Quik::getLastCandle(lua_State *luaState, const LastCandleRequestDto* lastCandleRequest) {
    return quikCandleService->getLastCandle(luaState, lastCandleRequest);
}

Option<TickerQuoteDto> Quik::getTickerQuotes(lua_State *luaState, string classCode, string ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {
        {STRING_TYPE, classCode, 0, 0.0, false},
        {STRING_TYPE, ticker, 0, 0.0, false}
    };

    if (!luaCallFunction(luaState, GET_QUOTE_LEVEL_2_FUNCTION_NAME, 2, 1, args)) {
        logError("Could not call QUIK %s function!", GET_QUOTE_LEVEL_2_FUNCTION_NAME);
        return Option<TickerQuoteDto>();
    }
    TickerQuoteDto tickerQuote;
    tickerQuote.classCode = classCode;
    tickerQuote.ticker = ticker;

    bool isSuccess = toTickerQuoteDto(luaState, &tickerQuote);

    if (isSuccess) {
        return Option<TickerQuoteDto>(tickerQuote);
    }
    logError("Could not get ticker quotes for class: %s and ticker: %s!", classCode.c_str(), ticker.c_str());

    return Option<TickerQuoteDto>();
}
