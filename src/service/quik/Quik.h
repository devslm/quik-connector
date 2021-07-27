//
// Created by Sergey on 24.06.2021.
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
#include "../../dto/candle/CandleSubscriptionDto.h"
#include "candle/QuikCandleService.h"
#include "order/QuikOrderService.h"
#include "../config/ConfigService.h"
#include "../../dto/ticker/TickerDto.h"
#include "../../dto/order/OrderDto.h"
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

const char QUIK_TRADES_TABLE_NAME[] = "trades";
const char QUIK_ORDERS_TABLE_NAME[] = "orders";

class ConfigService;
class QuikCandleService;
class QueueService;
class QuikOrderService;

extern ConfigService* configService;
extern shared_ptr<spdlog::logger> LOGGER;

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

        TradeDto trade;
        bool isSuccess = toAllTradeDto(L, &trade);

        allTradeLock.unlock();

        if (isSuccess) {
            trades.push(trade);
        } else {
            LOGGER->error("Could not handle all trade changes!");
        }
        return 0;
    }

    int onOrder(lua_State *luaState);

    void gcCollect(lua_State *luaState);

    Option<TradeDto> getNextTrade();

    int onQuote(lua_State *luaState);

    void message(lua_State *luaState, string text);

    Option<QuikConnectionStatusDto> getServerConnectionStatus(lua_State *luaState);

    bool isSubscribedToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    bool subscribeToCandles(lua_State *luaState, string classCode, string ticker, Interval interval);

    Option<QuikUserInfoDto> getUserName(lua_State *luaState);

    set<string> getClassesList(lua_State *luaState);

    Option<ClassInfoDto> getClassInfo(lua_State *luaState, string *className);

    Option<TickerQuoteDto> getTickerQuotes(lua_State *luaState, string classCode, string ticker);

    Option<CandleDto> getLastCandle(lua_State *luaState, const LastCandleRequestDto* lastCandleRequest);

    list<TradeDto> getTrades(lua_State *luaState);

    list<OrderDto> getNewOrders(lua_State *luaState);

    list<OrderDto> getOrders(lua_State *luaState);

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
