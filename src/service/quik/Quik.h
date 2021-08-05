//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
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
#include "../../dto/trade/TradeDto.h"
#include "../../dto/option/Option.h"
#include "../../mapper/trade/TradeMapper.h"
#include "../../mapper/quik/QuikServerMapper.h"
#include "../../mapper/quote/QuoteMapper.h"
#include "../utils/string/StringUtils.h"
#include "../../dto/class/ClassInfoDto.h"
#include "../../mapper/class/ClassInfoMapper.h"
#include "../../dto/connector/subscription/QuikSubscriptionDto.h"
#include "candle/QuikCandleService.h"
#include "order/QuikOrderService.h"
#include "../config/ConfigService.h"
#include "../../dto/ticker/TickerDto.h"
#include "../../dto/order/OrderDto.h"
#include "../../dto/order/StopOrderDto.h"
#include "../../mapper/ticker/TickerMapper.h"

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
const char SEND_TRANSACTION_FUNCTION_NAME[] = "sendTransaction";

const char QUIK_TRADES_TABLE_NAME[] = "trades";
const char QUIK_ORDERS_TABLE_NAME[] = "orders";
const char QUIK_STOP_ORDERS_TABLE_NAME[] = "stop_orders";

class ConfigService;
class QuikCandleService;
class QueueService;
class QuikOrderService;

class Quik {
public:
    Quik();

    virtual ~Quik();

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
            LOGGER->error("Could not handle all trade changes!");
        }
        allTradeLock.unlock();
        changedTradeQueueLock.unlock();

        return 0;
    }

    int onOrder(lua_State *luaState);

    void gcCollect(lua_State *luaState);

    int onQuote(lua_State *luaState);

    int onTransReply(lua_State *luaState);

    void message(lua_State *luaState, string text);

    Option<QuikConnectionStatusDto> getServerConnectionStatus(lua_State *luaState);

    bool isSubscribedToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    bool subscribeToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    Option<QuikUserInfoDto> getUserName(lua_State *luaState);

    set<string> getClassesList(lua_State *luaState);

    Option<ClassInfoDto> getClassInfo(lua_State *luaState, string *className);

    Option<TickerQuoteDto> getTickerQuotes(lua_State *luaState, string classCode, string ticker);

    Option<CandleDto> getLastCandle(lua_State *luaState, CandlesRequestDto& candlesRequest);

    Option<CandleDto> getCandles(lua_State *luaState, CandlesRequestDto& candlesRequest);

    list<TradeDto> getTrades(lua_State *luaState);

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

    list<StopOrderDto> getStopOrders(lua_State *luaState);

    bool cancelStopOrderById(lua_State *luaState, CancelStopOrderRequestDto& cancelStopOrderRequest);

    Option<TickerDto> getTickerById(lua_State *luaState, string classCode, string tickerCode);

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
