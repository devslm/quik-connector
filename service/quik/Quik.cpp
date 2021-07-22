//
// Created by Sergey on 24.06.2021.
//

#include "Quik.h"

Quik::Quik() {
    this->mutexLock = luaGetMutex();
}

Quik::~Quik() {

}

bool Quik::isRunning() const {
    return isConnectorRunning;
}

int Quik::onStart(lua_State *luaState) {
    string logMessage = APP_NAME + " started";
    auto config = configService->getConfig();

    this->queueService = new QueueService(this, config.redis.host, config.redis.port);
    this->queueService->subscribe();

    this->quikCandleService = new QuikCandleService(this->queueService);
    this->quikCandleService->init();

    this->quikOrderService = new QuikOrderService(this);

    isConnectorRunning = true;

    this->checkQuotesThread = thread([this] {startCheckQuotesThread();});
    this->checkNewOrdersThread = thread([this] {startCheckNewOrdersThread();});
    this->isConnectorRunning = true;

    logInfo(logMessage.c_str());
    message(luaState, logMessage);

    return 0;
}

int Quik::onStop(lua_State *luaState) {
    isConnectorRunning = false;
    string logMessage = APP_NAME + " stopped";

    quikCandleService->destroy();

    delete quikOrderService;
    delete queueService;

    checkQuotesThread.join();
    checkNewOrdersThread.join();

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
        logError("Could not handle all trade changes!");
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
        logError("Could not get changed quote class code");
        return 0;
    }
    isSuccess = luaGetString(luaState, &ticker);

    if (!isSuccess) {
        logError("Could not get changed quote ticker");
        return 0;
    }
    changedQuotes[ticker] = classCode;

    quoteLock.unlock();
    changedQuoteMapLock.unlock();

    return 0;
}

void Quik::startCheckQuotesThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(2));

        changedQuoteMapLock.lock();

        unordered_map<string, string> quotes(changedQuotes);

        changedQuotes.clear();

        changedQuoteMapLock.unlock();

        for (const auto& entry : quotes) {
            string ticker = entry.first;
            string classCode = entry.second;
            auto tickerQuotes = getTickerQuotes(luaGetState(), ticker, classCode);

            //logInfo("New quotes: %s", toTickerQuoteJson(&tickerQuotes).dump().c_str());
        }
    }
}

int Quik::onOrder(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    orderLock.lock();
    changedOrderListLock.lock();

    OrderDto order;

    bool isSuccess = toOrderDto(luaState, this, &order);

    orderLock.unlock();
    changedOrderListLock.unlock();

    if (isSuccess) {
        newOrders.push_back(order);
    } else {
        logError("Could not get new order from callback!");
    }
    return 0;
}

void Quik::startCheckNewOrdersThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(100));

        changedOrderListLock.lock();

        list<OrderDto> orders(newOrders);

        newOrders.clear();

        changedOrderListLock.unlock();

        for (const auto& order : orders) {
            Option<OrderDto> orderOption(order);

            logInfo("New order received: %s", toOrderJson(orderOption).dump().c_str());
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

list<TradeDto> Quik::getTrades(lua_State *luaState) {
    list<TradeDto> existsTrades;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{STRING_TYPE, QUIK_TRADES_TABLE_NAME, 0, 0.0, false}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logError("Could not call QUIK %s function!", GET_NUMBER_OF_FUNCTION_NAME);
        return existsTrades;
    }
    double totalOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalOrders);

    if (!isSuccess) {
        logError("Could not get trades because can't retrieve total trades number!");
        return existsTrades;
    }
    logDebug("Found: %d trades", (int)totalOrders);

    for (int i = 0; i < totalOrders; ++i) {
        FunctionArgDto args[] = {
            {STRING_TYPE,  QUIK_TRADES_TABLE_NAME, 0, 0.0, false},
            {INTEGER_TYPE, "",                     i, 0.0, false}
        };

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, args)) {
            logError("Could not call QUIK %s function!", GET_ITEM_FUNCTION_NAME);
            return existsTrades;
        }
        TradeDto trade;

        isSuccess = toTradeDto(luaState, &trade);

        if (!isSuccess) {
            logError("Could not convert trade data to dto!");
            continue;
        }
        Option<TradeDto> tradeOption(trade);

        logDebug("Trade: %s", toTradeJson(&tradeOption).c_str());

        existsTrades.push_back(trade);
    }
    return existsTrades;
}

list<OrderDto> Quik::getNewOrders(lua_State *luaState) {
    return quikOrderService->getNewOrders(luaState);
}

list<OrderDto> Quik::getOrders(lua_State *luaState) {
    return quikOrderService->getOrders(luaState);
}

Option<TickerDto> Quik::getTickerById(lua_State *luaState, string classCode, string tickerCode) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {
        {STRING_TYPE, classCode, 0, 0.0, false},
        {STRING_TYPE, tickerCode, 0, 0.0, false}
    };

    if (!luaCallFunction(luaState, GET_SECURITY_INFO_FUNCTION_NAME, 2, 1, args)) {
        logError("Could not call QUIK %s function!", GET_SECURITY_INFO_FUNCTION_NAME);
        return Option<TickerDto>();
    }
    TickerDto ticker;

    bool isSuccess = toTickerDto(luaState, &ticker);

    if (isSuccess) {
        return Option<TickerDto>(ticker);
    }
    logError("Could not get ticker info for class: %s and ticker: %s!", classCode.c_str(), tickerCode.c_str());

    return Option<TickerDto>();
}
