//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "Quik.h"

Quik::Quik() {
    this->mutexLock = luaGetMutex();
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

    //this->quikNewsService = new QuikNewsService(this);
    //this->quikNewsService->startMonitorUpdates(luaState);

    isConnectorRunning = true;

    this->checkAllTradesThread = thread([this] {startCheckAllTradesThread();});
    this->checkQuotesThread = thread([this] {startCheckQuotesThread();});
    this->checkNewOrdersThread = thread([this] {startCheckNewOrdersThread();});
    this->pushServerInfoThread = thread([this] {startPushServerInfoThread();});
    this->isConnectorRunning = true;

    logger->info(logMessage);
    logger->info("QUIK version: {}", getVersion(luaState).get());

    message(luaState, logMessage);

    return 0;
}

int Quik::onStop(lua_State *luaState) {
    isConnectorRunning = false;
    string logMessage = APP_NAME + " stopped";

    quikCandleService->destroy();

    checkAllTradesThread.join();
    checkQuotesThread.join();
    checkNewOrdersThread.join();
    pushServerInfoThread.join();

    delete quikOrderService;
    delete queueService;
    //delete quikNewsService;

    logger->info(logMessage);

    message(luaState, logMessage, Quik::MessageIconType::WARNING);

    return 0;
}

void Quik::startCheckAllTradesThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (trades.empty()) {
            continue;
        }
        list<TradeDto> tradeList;

        changedTradeQueueLock.lock();

        while (!trades.empty()) {
            tradeList.push_back(trades.front());

            trades.pop();
        }
        changedTradeQueueLock.unlock();

        for (const auto& trade : tradeList) {
            Option<TradeDto> tradeOption(trade);

            SuccessResponseDto response(toAllTradeJson(tradeOption));

            queueService->pubSubPublish(QueueService::QUIK_ALL_TRADES_TOPIC, response);
        }
    }
}

int Quik::onQuote(lua_State *luaState) {
    lock_guard<recursive_mutex> mutexLockGuard(*mutexLock);

    string classCode;
    string ticker;

    bool isSuccess = luaGetString(luaState, &classCode);

    if (!isSuccess) {
        logger->error("Could not get changed quote class code");
        return 0;
    }
    isSuccess = luaGetString(luaState, &ticker);

    if (!isSuccess) {
        logger->error("Could not get changed quote ticker");
        return 0;
    }
    changedQuotes.put(ticker, classCode);

    return 0;
}

void Quik::startCheckQuotesThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(2));

        if (changedQuotes.isEmpty()) {
            continue;
        }

        for (const auto& entry : changedQuotes.iterator()) {
            string ticker = entry.key;
            string classCode = entry.value;

            changedQuotes.remove(entry.key);

            auto tickerQuotes = getTickerQuotes(luaGetState(), ticker, classCode);

            SuccessResponseDto response(toTickerQuoteJson(tickerQuotes));

            queueService->pubSubPublish(QueueService::QUIK_TICKER_QUOTES_TOPIC, response);
        }
    }
}

int Quik::onOrder(lua_State *luaState) {
    lock_guard<recursive_mutex> mutexLockGuard(*mutexLock);
    lock_guard<mutex> orderLockGuard(orderLock);
    lock_guard<mutex> changedOrderListLockGuard(changedOrderListLock);

    OrderDto order;

    bool isSuccess = toOrderDto(luaState, this, &order);

    if (isSuccess) {
        newOrders.push_back(order);
    } else {
        logger->error("Could not get new order from callback!");
    }
    return 0;
}

int Quik::onTransReply(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    TransactionReplyDto transactionReply;
    bool isSuccess = toTransactionReplyDto(luaState, &transactionReply);

    if (isSuccess) {
        Option<TransactionReplyDto> transactionReplyOption(transactionReply);

        logger->info("Trans reply: {}", toTransactionReplyJson(transactionReplyOption).dump());
    } else {
        logger->info("Could not read trans reply!");
    }
    return 0;
}

void Quik::startCheckNewOrdersThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::milliseconds(2));

        if (newOrders.empty()) {
            continue;
        }
        lock_guard<mutex> changedOrderListLockGuard(changedOrderListLock);

        OrderDto order = newOrders.front();

        newOrders.pop_front();

        OrderEntity orderEntity = toOrderEntity(order);

        quikOrderService->onNewOrder(order);
    }
}

void Quik::startPushServerInfoThread() {
    while (isConnectorRunning) {
        this_thread::sleep_for(chrono::seconds(1));

        Option<string> ping = getAvgPingDuration(luaGetState());
        Option<string> serverTime = getServerTime(luaGetState());
        Option<QuikConnectionStatusDto> connectionStatus = getServerConnectionStatus(luaGetState());

        json jsonObject;
        jsonObject["ping"] = (ping.isPresent() ? ping.get() : nullptr);
        jsonObject["serverTime"] = (serverTime.isPresent() ? serverTime.get() : nullptr);
        jsonObject["isConnected"] = (connectionStatus.isPresent() ? connectionStatus.get().isConnected : nullptr);

        SuccessResponseDto response(jsonObject);

        queueService->pubSubPublish(QueueService::QUIK_SERVER_INFO_TOPIC, response);
    }
}

void Quik::gcCollect(lua_State *luaState) {
    luaGcCollect(luaState);
}

bool Quik::message(lua_State *luaState, string& text, Quik::MessageIconType iconType) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{text}, {iconType}};

    bool isSuccessDisplayed = false;

    if (!luaCallFunction(luaState, MESSAGE_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", MESSAGE_FUNCTION_NAME);
    } else {
        double result = 0.0;

        if (luaGetNumber(luaState, &result) && result >= 1.0) {
            isSuccessDisplayed = true;
        }
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return isSuccessDisplayed;
}

Option<string> Quik::getWorkingFolder(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaCallFunction(luaState, GET_WORKING_FOLDER_FUNCTION_NAME, 0, 1, nullptr)) {
        logger->error("Could not call QUIK {} function!", GET_WORKING_FOLDER_FUNCTION_NAME);
        return {};
    }
    string quikFolder;

    if (luaGetString(luaState, &quikFolder)) {
        return {quikFolder};
    }
    logger->error("Could not get QUIK working folder! Reason: {}", luaGetErrorMessage(luaState));

    return {};
}

Option<QuikConnectionStatusDto> Quik::getServerConnectionStatus(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaCallFunction(luaState, IS_CONNECTED_FUNCTION_NAME, 0, 1, nullptr)) {
        logger->error("Could not call QUIK {} function!", IS_CONNECTED_FUNCTION_NAME);
        return {};
    }
    QuikConnectionStatusDto quikConnectionStatus;

    bool isSuccess = toQuikServerConnectionStatusDto(luaState, &quikConnectionStatus);

    if (isSuccess) {
        return {quikConnectionStatus};
    }
    return {};
}

Option<QuikUserInfoDto> Quik::getUserName(lua_State *luaState) {
    Option<string> userName = getInfoParam(luaState, "USER");

    if (userName.isPresent()) {
        QuikUserInfoDto quikUserInfo;
        quikUserInfo.name = userName.get();

        return {quikUserInfo};
    }
    return {};
}

Option<string> Quik::getVersion(lua_State *luaState) {
    return getInfoParam(luaState, "VERSION");
}

Option<string> Quik::getServerTime(lua_State *luaState) {
    return getInfoParam(luaState, "SERVERTIME");
}

Option<string> Quik::getAvgPingDuration(lua_State *luaState) {
    return getInfoParam(luaState, "AVGPINGDURATION");
}

Option<string> Quik::getInfoParam(lua_State *luaState, const string& paramName) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{paramName}};

    if (!luaCallFunction(luaState, GET_INFO_PARAM_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function to get info param!", GET_INFO_PARAM_FUNCTION_NAME);
        return {};
    }
    string paramValue;

    bool isSuccess = luaGetString(luaState, &paramValue);

    if (isSuccess) {
        return {paramValue};
    }
    logger->error("Could not get info param for param: {}", paramName);

    return {};
}

Option<DepoLimitDto> Quik::getDepoLimit(lua_State *luaState,
                                        string& clientCode,
                                        string& firmId,
                                        string& ticker,
                                        string& account,
                                        int limitKind) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{clientCode}, {firmId}, {ticker}, {account}, {limitKind}};

    if (!luaCallFunction(luaState, GET_DEPO_EX_FUNCTION_NAME, 5, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_DEPO_EX_FUNCTION_NAME);
        return {};
    }
    DepoLimitDto depoLimit;

    bool isSuccess = toDepoLimitDto(luaState, &depoLimit);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {depoLimit};
    }
    logger->error("Could not get depo limit for client code: {}, firm: {}, ticker: {} and account: {}",
        clientCode, firmId, ticker, account);

    return {};
}

set<string> Quik::getClientCodes(lua_State *luaState) {
    set<string> clientCodes;

    getTableValues(luaState, QUIK_CLIENT_CODES_TABLE_NAME, [&clientCodes](string& clientCode) {
        if (!clientCode.empty()) {
            clientCodes.insert(clientCode);
        }
    });
    return clientCodes;
}

void Quik::getTableValues(lua_State *luaState, const string& tableName, function<void(string&)> const& callback) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{tableName}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function for: {}!", GET_NUMBER_OF_FUNCTION_NAME, tableName);
        return;
    }
    double totalItems = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalItems);

    if (!isSuccess) {
        logger->error("Can't retrieve total items number for: {}!", tableName);
        return;
    }
    logger->debug("Found total items: {} for: {}", totalItems, tableName);

    for (int i = 0; i < totalItems; ++i) {
        FunctionArgDto getItemArgs[] = {{tableName}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, getItemArgs)) {
            logger->error("Could not call QUIK {} function for: {}!", GET_ITEM_FUNCTION_NAME, tableName);
            return;
        }
        string clientCode;

        isSuccess = luaGetString(luaState, &clientCode);

        if (!isSuccess) {
            logger->error("Could not get item for: {}!", tableName);
            continue;
        }
        callback(clientCode);
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);
}

list<TradeAccountDto> Quik::getTradeAccounts(lua_State *luaState) {
    list<TradeAccountDto> tradeAccounts;
    string tableName = QUIK_TRADE_ACCOUNTS_TABLE_NAME;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{tableName}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function for: {}!", GET_NUMBER_OF_FUNCTION_NAME, tableName);
        return tradeAccounts;
    }
    double totalItems = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalItems);

    if (!isSuccess) {
        logger->error("Can't retrieve total items number for: {}!", tableName);
        return tradeAccounts;
    }
    logger->debug("Found total items: {} for: {}", totalItems, tableName);

    for (int i = 0; i < totalItems; ++i) {
        FunctionArgDto getItemArgs[] = {{tableName}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, getItemArgs)) {
            logger->error("Could not call QUIK {} function for: {}!", GET_ITEM_FUNCTION_NAME, tableName);
            return tradeAccounts;
        }
        TradeAccountDto tradeAccount;

        if (toTradeAccountDto(luaState, &tradeAccount)) {
            tradeAccounts.push_back(tradeAccount);
        } else {
            logger->error("Could not convert trade account data to dto with table index: {}", i);
        }
    }
    return tradeAccounts;
}

set<string> Quik::getClassesList(lua_State *luaState) {
    set<string> classes;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    if (!luaCallFunction(luaState, GET_CLASSES_LIST_FUNCTION_NAME, 0, 1, nullptr)) {
        logger->error("Could not call QUIK {} function!", GET_CLASSES_LIST_FUNCTION_NAME);
        return classes;
    }
    string classesStr;

    bool isSuccess = luaGetString(luaState, &classesStr);

    if (isSuccess) {
        classes = stringSplitByDelimeter(classesStr, ',');
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return classes;
}

Option<ClassInfoDto> Quik::getClassInfo(lua_State *luaState, string& className) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{className}};

    if (!luaCallFunction(luaState, GET_CLASS_INFO_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_CLASS_INFO_FUNCTION_NAME);
        return {};
    }
    ClassInfoDto classInfo;
    bool isSuccess = toClassInfoDto(luaState, &classInfo);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {classInfo};
    }
    logger->error("Could not get class info for: {}!", className);

    return {};
}

set<string> Quik::getClassSecurities(lua_State *luaState, string& className) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    set<string> classSecurities;
    FunctionArgDto args[] = {{className}};

    if (!luaCallFunction(luaState, GET_CLASS_SECURITIES_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_CLASS_SECURITIES_FUNCTION_NAME);
        return classSecurities;
    }
    string classSecuritiesStr;

    bool isSuccess = luaGetString(luaState, &classSecuritiesStr);

    if (isSuccess) {
        classSecurities = stringSplitByDelimeter(classSecuritiesStr, ',');
    } else {
        logger->error("Could not get class securities for class code: {}!", className);
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return classSecurities;
}

Option<MoneyLimitDto> Quik::getMoney(lua_State *luaState,
                                     string& clientCode,
                                     string& firmId,
                                     string& tag,
                                     string& currencyCode) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{clientCode}, {firmId}, {tag}, {currencyCode}};

    if (!luaCallFunction(luaState, GET_MONEY_FUNCTION_NAME, 4, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_MONEY_FUNCTION_NAME);
        return {};
    }
    MoneyLimitDto moneyLimit;

    bool isSuccess = toMoneyLimitDto(luaState, &moneyLimit);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {moneyLimit};
    }
    logger->error("Could not get money limit with client code: {}, firm: {}, tag: {} and currency code: {}!",
        clientCode, firmId, tag, currencyCode);

    return {};
}

Option<FutureLimitDto> Quik::getFuturesLimit(lua_State *luaState,
                                             string& firmId,
                                             string& account,
                                             int limitType,
                                             string& currencyCode) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{firmId}, {account}, {limitType}, {currencyCode}};

    if (!luaCallFunction(luaState, GET_FUTURES_LIMIT_FUNCTION_NAME, 4, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_FUTURES_LIMIT_FUNCTION_NAME);
        return {};
    }
    FutureLimitDto futureLimit;

    auto isSuccess = toFutureLimitDto(luaState, &futureLimit);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {futureLimit};
    }
    logger->error("Could not get futures limit with firm: {}, account: {}, limit type: {} and currency code: {}!",
        firmId, account, limitType, currencyCode);

    return {};
}

Option<MaxTradeLotsDto> Quik::calcBuySell(lua_State *luaState,
                                          string& classCode,
                                          string& ticker,
                                          string& clientCode,
                                          string& account,
                                          double price,
                                          bool isBuy,
                                          bool isMarket) {
    // See this limitation in the ARQA QUIK doc
    if (isMarket) {
        price = 0.0;
    }
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}, {clientCode}, {account}, {price}, {isBuy}, {isMarket}};

    if (!luaCallFunction(luaState, CALC_BUY_SELL_FUNCTION_NAME, 7, 2, args)) {
        logger->error("Could not call QUIK {} function!", CALC_BUY_SELL_FUNCTION_NAME);
        return {};
    }
    MaxTradeLotsDto maxTradeLots;

    auto isSuccess = toMaxTradeLotsDto(luaState, &maxTradeLots);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {maxTradeLots};
    }
    logger->error("Could not calculate max buy/sell lots with clientCode: {}, account: {}, class code: {}, ticker: {}, price: {}, is buy: {} and is market: {}!",
        clientCode, account, classCode, ticker, price, isBuy, isMarket);

    return {};
}

bool Quik::isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return quikCandleService->isSubscribedToCandles(luaState, classCode, ticker, interval);
}

bool Quik::subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return quikCandleService->subscribeToCandles(luaState, classCode, ticker, interval);
}

bool Quik::subscribeToCandles(lua_State *luaState,
                              string& classCode,
                              string& ticker,
                              Interval& interval,
                              Option<UpdateCandleCallback>& updateCandleCallback) {
    return quikCandleService->subscribeToCandles(luaState, classCode, ticker, interval, updateCandleCallback);
}

bool Quik::unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval) {
    return quikCandleService->unsubscribeFromCandles(luaState, classCode, ticker, interval);
}

bool Quik::isSubscribedToTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    return quikCandleService->isSubscribedToTickerQuotes(luaState, classCode, ticker);
}

bool Quik::subscribeToTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    return quikCandleService->subscribeToTickerQuotes(luaState, classCode, ticker);
}

bool Quik::unsubscribeFromTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    return quikCandleService->unsubscribeFromTickerQuotes(luaState, classCode, ticker);
}

bool Quik::getCandles(lua_State *luaState, CandlesRequestDto& candlesRequest) {
    return quikCandleService->getCandles(luaState, candlesRequest);
}

bool Quik::getCandles(lua_State *luaState, CandlesRequestDto& candlesRequest, Option<CandlesReadyCallback>& candlesReadyCallback) {
    return quikCandleService->getCandles(luaState, candlesRequest, candlesReadyCallback);
}

Option<TickerQuoteDto> Quik::getTickerQuotes(lua_State *luaState, string& classCode, string& ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}};

    if (!luaCallFunction(luaState, GET_QUOTE_LEVEL_2_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_QUOTE_LEVEL_2_FUNCTION_NAME);
        return {};
    }
    TickerQuoteDto tickerQuote;
    tickerQuote.classCode = classCode;
    tickerQuote.ticker = ticker;

    bool isSuccess = toTickerQuoteDto(luaState, &tickerQuote);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {tickerQuote};
    }
    logger->error("Could not get ticker quotes for class: {} and ticker: {}!", classCode, ticker);

    return {};
}

list<TradeDto> Quik::getTrades(lua_State *luaState) {
    list<TradeDto> existsTrades;

    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{QUIK_TRADES_TABLE_NAME}};

    if (!luaCallFunction(luaState, GET_NUMBER_OF_FUNCTION_NAME, 1, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_NUMBER_OF_FUNCTION_NAME);
        return existsTrades;
    }
    double totalOrders = 0.0;
    bool isSuccess = luaGetNumber(luaState, &totalOrders);

    if (!isSuccess) {
        logger->error("Could not get trades because can't retrieve total trades number!");
        return existsTrades;
    }
    logger->debug("Found: {} trades", totalOrders);

    for (int i = 0; i < totalOrders; ++i) {
        FunctionArgDto getItemArgs[] = {{QUIK_TRADES_TABLE_NAME}, {i}};

        if (!luaCallFunction(luaState, GET_ITEM_FUNCTION_NAME, 2, 1, getItemArgs)) {
            logger->error("Could not call QUIK {} function!", GET_ITEM_FUNCTION_NAME);
            return existsTrades;
        }
        TradeDto trade;

        isSuccess = toTradeDto(luaState, &trade);

        if (!isSuccess) {
            logger->error("Could not convert trade data to dto!");
            continue;
        }
        Option<TradeDto> tradeOption(trade);

        existsTrades.push_back(trade);
    }
    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return existsTrades;
}

list<OrderDto> Quik::getNewOrders(lua_State *luaState) {
    return quikOrderService->getNewOrders(luaState);
}

list<OrderDto> Quik::getOrders(lua_State *luaState) {
    return quikOrderService->getOrders(luaState);
}

list<StopOrderDto> Quik::getStopOrders(lua_State *luaState) {
    return quikOrderService->getStopOrders(luaState);
}

Option<OrderDto> Quik::getOrderById(lua_State *luaState, uint64_t orderId) {
    return quikOrderService->getOrderById(luaState, orderId);
}

bool Quik::cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest) {
    return quikOrderService->cancelStopOrderById(luaState, cancelStopOrderRequest);
}

list<TickerDto> Quik::getTickersByClassCode(lua_State *luaState, string& classCode) {
    logger->info("Get tickers with class code: {}", classCode);

    auto tickerCodes = getClassSecurities(luaState, classCode);
    list<TickerDto> tickers;

    for (auto tickerCode : tickerCodes) {
        auto ticker = getTickerById(luaState, classCode, tickerCode);

        if (ticker.isPresent()) {
            tickers.push_back(ticker.get());
        }
    }
    logger->info("Found: {} tickers with class code: {}", tickers.size(), classCode);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return tickers;
}

Option<TickerDto> Quik::getTickerById(lua_State *luaState, string& classCode, string& tickerCode) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {tickerCode}};

    if (!luaCallFunction(luaState, GET_SECURITY_INFO_FUNCTION_NAME, 2, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_SECURITY_INFO_FUNCTION_NAME);
        return {};
    }
    TickerDto ticker;

    bool isSuccess = toTickerDto(luaState, &ticker);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {ticker};
    }
    logger->error("Could not get ticker info for class: {} and ticker: {}!", classCode, tickerCode);

    return {};
}

Option<double> Quik::getTickerPriceStepCost(lua_State *luaState, const string& classCode, const string& ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    auto paramOption = getParamEx(luaState, classCode, ticker, "STEPPRICE");

    if (paramOption.isPresent()) {
        ParamDto param = paramOption.get();

        if (PARAM_DOUBLE_TYPE == param.paramType) {
            Option<double> priceStep = getTickerPriceStep(luaState, classCode, ticker);

            if (priceStep.isPresent()) {
                return stod(param.paramValue) / priceStep.get();
            } else {
                logger->error("Could not get ticker price step cost for class code: {} and ticker: {} because no price step value!",
                    classCode, ticker, param.paramType + 1);
                return {};
            }
        } else {
            logger->error("Could not get ticker price step cost for class code: {} and ticker: {}! Reason: Param type should be double but found: {}",
                classCode, ticker, param.paramType + 1);
            return {};
        }
    }
    logger->error("Could not get ticker price step cost for class code: {} and ticker: {}!", classCode, ticker);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return {};
}

Option<double> Quik::getTickerPriceStep(lua_State *luaState, const string& classCode, const string& ticker) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    Option<ParamDto> paramOption = getParamEx(luaState, classCode, ticker, "SEC_PRICE_STEP");

    if (paramOption.isPresent()) {
        ParamDto param = paramOption.get();

        if (PARAM_DOUBLE_TYPE == param.paramType) {
            return stod(param.paramValue);
        } else {
            logger->error("Could not get ticker price step for class code: {} and ticker: {}! Reason: Param type should be double but found: {}",
                classCode, ticker, param.paramType + 1);
            return {};
        }
    }
    logger->error("Could not get ticker price step for class code: {} and ticker: {}!", classCode, ticker);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return {};
}

Option<ParamDto> Quik::getParamEx(lua_State *luaState,
                                  const string& classCode,
                                  const string& ticker,
                                  const string& paramName) {
    lock_guard<recursive_mutex> lockGuard(*mutexLock);

    FunctionArgDto args[] = {{classCode}, {ticker}, {paramName}};

    if (!luaCallFunction(luaState, GET_PARAM_EX_FUNCTION_NAME, 3, 1, args)) {
        logger->error("Could not call QUIK {} function!", GET_PARAM_EX_FUNCTION_NAME);
        return {};
    }
    ParamDto param;

    auto isSuccess = toParamDto(luaState, &param);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    if (isSuccess) {
        return {param};
    }
    logger->error("Could not get param ex value for class code: {}, ticker: {} and param: {}", classCode, ticker, paramName);

    return {};
}
