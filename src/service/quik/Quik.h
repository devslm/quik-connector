//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIK_H
#define QUIK_CONNECTOR_QUIK_H

#include <cstdint>
#include <queue>
#include <set>
#include <unordered_map>
#include <mutex>
#include <string>
#include "utils/QuikUtils.h"
#include "../lua/Lua.h"
#include "../../dto/config/Config.h"
#include "../../dto/quik/trade/TradeDto.h"
#include "../../dto/option/Option.h"
#include "../../mapper/quik/trade/TradeMapper.h"
#include "../../mapper/quik/server/QuikServerMapper.h"
#include "../../mapper/quik/quote/QuoteMapper.h"
#include "../../mapper/quik/transaction/TransactionMapper.h"
#include "../../mapper/quik/depo/DepoLimitMapper.h"
#include "../../mapper/quik/param/ParamMapper.h"
#include "../utils/string/StringUtils.h"
#include "../../dto/quik/class/ClassInfoDto.h"
#include "../../mapper/quik/class/ClassInfoMapper.h"
#include "../../dto/quik/connector/subscription/QuikSubscriptionDto.h"
#include "candle/QuikCandleService.h"
#include "order/QuikOrderService.h"
#include "../config/ConfigService.h"
#include "../../dto/quik/ticker/TickerDto.h"
#include "../../dto/quik/order/OrderDto.h"
#include "../../dto/quik/order/StopOrderDto.h"
#include "../../mapper/quik/ticker/TickerMapper.h"
#include "../../dto/quik/limit/MoneyLimitDto.h"
#include "../../mapper/quik/money/MoneyLimitMapper.h"
#include "../../mapper/quik/future/FutureLimitMapper.h"

using namespace std;

const char MESSAGE_FUNCTION_NAME[] = "message";
const char IS_CONNECTED_FUNCTION_NAME[] = "isConnected";
const char GET_CLASSES_LIST_FUNCTION_NAME[] = "getClassesList";
const char GET_CLASS_INFO_FUNCTION_NAME[] = "getClassInfo";
const char GET_INFO_PARAM_FUNCTION_NAME[] = "getInfoParam";
const char GET_QUOTE_LEVEL_2_FUNCTION_NAME[] = "getQuoteLevel2";
const char GET_NUMBER_OF_FUNCTION_NAME[] = "getNumberOf";
const char GET_ITEM_FUNCTION_NAME[] = "getItem";
const char GET_SECURITY_INFO_FUNCTION_NAME[] = "getSecurityInfo";
const char GET_CLASS_SECURITIES_FUNCTION_NAME[] = "getClassSecurities";
const char GET_MONEY_FUNCTION_NAME[] = "getMoney";
const char GET_FUTURES_LIMIT_FUNCTION_NAME[] = "getFuturesLimit";
const char GET_DEPO_EX_FUNCTION_NAME[] = "getDepoEx";
const char GET_PARAM_EX_FUNCTION_NAME[] = "getParamEx";
const char SEND_TRANSACTION_FUNCTION_NAME[] = "sendTransaction";
const char SUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME[] = "Subscribe_Level_II_Quotes";
const char UNSUBSCRIBE_LEVEL_2_QUOTES_FUNCTION_NAME[] = "Subscribe_Level_II_Quotes";
const char IS_SUBSCRIBED_LEVEL_2_QUOTES_FUNCTION_NAME[] = "IsSubscribed_Level_II_Quotes";

const char QUIK_TRADES_TABLE_NAME[] = "trades";
const char QUIK_ORDERS_TABLE_NAME[] = "orders";
const char QUIK_STOP_ORDERS_TABLE_NAME[] = "stop_orders";
const char QUIK_CLIENT_CODES_TABLE_NAME[] = "client_codes";

class ConfigService;
class QuikCandleService;
class QueueService;
class QuikOrderService;

class Quik {
public:
    Quik();

    ~Quik() = default;

    bool isRunning() const;

    int onStart(lua_State *luaState);

    int onStop(lua_State *luaState);

    __forceinline int onAllTrade(lua_State *L) {
        lock_guard<recursive_mutex> lockGuard(*mutexLock);

        allTradeLock.lock();
        changedTradeQueueLock.lock();

        TradeDto trade;
        bool isSuccess = toAllTradeDto(L, &trade);

        if (isSuccess) {
            trades.push(trade);
        } else {
            logger->error("Could not handle all trade changes!");
        }
        allTradeLock.unlock();
        changedTradeQueueLock.unlock();

        return 0;
    }

    int onOrder(lua_State *luaState);

    void gcCollect(lua_State *luaState);

    int onQuote(lua_State *luaState);

    int onTransReply(lua_State *luaState);

    void message(lua_State *luaState, string& text);

    Option<QuikConnectionStatusDto> getServerConnectionStatus(lua_State *luaState);

    bool isSubscribedToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool subscribeToCandles(lua_State *luaState,
                            string& classCode,
                            string& ticker,
                            Interval& interval,
                            Option<UpdateCandleCallback>& updateCandleCallback);

    bool unsubscribeFromCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

    bool isSubscribedToTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    bool subscribeToTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    bool unsubscribeFromTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    Option<QuikUserInfoDto> getUserName(lua_State *luaState);

    Option<string> getVersion(lua_State *luaState);

    Option<string> getServerTime(lua_State *luaState);

    Option<string> getAvgPingDuration(lua_State *luaState);

    Option<string> getInfoParam(lua_State *luaState, const string& paramName);

    Option<ParamDto> getParamEx(lua_State *luaState, const string& classCode, const string& ticker, const string& paramName);

    Option<DepoLimitDto> getDepoLimit(lua_State *luaState,
                                      string& clientCode,
                                      string& firmId,
                                      string& ticker,
                                      string& account,
                                      int limitKind);

    set<string> getClientCodes(lua_State *luaState);

    void getTableValues(lua_State *luaState, const string& tableName, function<void(string&)> const& callback);

    set<string> getClassesList(lua_State *luaState);

    Option<ClassInfoDto> getClassInfo(lua_State *luaState, string& className);

    set<string> getClassSecurities(lua_State *luaState, string& className);

    Option<MoneyLimitDto> getMoney(lua_State *luaState, string& clientCode, string& firmId, string& tag, string& currencyCode);

    Option<FutureLimitDto> getFuturesLimit(lua_State *luaState, string& firmId, string& account, int limitType, string& currencyCode);

    Option<TickerQuoteDto> getTickerQuotes(lua_State *luaState, string& classCode, string& ticker);

    Option<CandleDto> getLastCandle(lua_State *luaState, CandlesRequestDto& candlesRequest);

    Option<CandleDto> getCandles(lua_State *luaState, CandlesRequestDto& candlesRequest);

    list<TradeDto> getTrades(lua_State *luaState);

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

    list<StopOrderDto> getStopOrders(lua_State *luaState);

    bool cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest);

    list<TickerDto> getTickersByClassCode(lua_State *luaState, string& classCode);

    Option<TickerDto> getTickerById(lua_State *luaState, string& classCode, string& tickerCode);

    Option<double> getTickerPriceStepCost(lua_State *luaState, const string& classCode, const string& tickerCode);

    Option<double> getTickerPriceStep(lua_State *luaState, const string& classCode, const string& ticker);

private:
    bool isConnectorRunning = false;
    queue<TradeDto> trades;
    recursive_mutex* mutexLock;
    mutex allTradeLock;
    mutex quoteLock;
    mutex orderLock;
    mutex changedQuoteMapLock;
    mutex changedOrderListLock;
    mutex changedTradeQueueLock;
    thread checkAllTradesThread;
    thread checkQuotesThread;
    thread checkNewOrdersThread;
    unordered_map<string, string> changedQuotes;
    list<OrderDto> newOrders;
    QuikCandleService *quikCandleService;
    QuikOrderService *quikOrderService;
    QueueService *queueService;

    void startCheckAllTradesThread();

    void startCheckQuotesThread();

    void startCheckNewOrdersThread();
};

#endif //QUIK_CONNECTOR_QUIK_H
