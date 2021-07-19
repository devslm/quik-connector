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
#include "../../service/quik/utils/QuikUtils.h"
#include "../../service/lua/Lua.h"
#include "../../dto/config/Config.h"
#include "../../dto/trade/TradeDto.h"
#include "../../dto/option/Option.h"
#include "../../mapper/trade/TradeMapper.h"
#include "../../mapper/quik/QuikServerMapper.h"
#include "../../mapper/quote/QuoteMapper.h"
#include "../../service/utils/string/StringUtils.h"
#include "../../dto/class/ClassInfoDto.h"
#include "../../mapper/class/ClassInfoMapper.h"
#include "../../dto/candle/CandleSubscriptionDto.h"
#include "candle/QuikCandleService.h"
#include "../../service/config/ConfigService.h"

using namespace std;

class ConfigService;
class QuikCandleService;
class QueueService;

extern ConfigService *configService;

class Quik {
public:
    Quik();

    virtual ~Quik();

    bool isRunning() const;

    int onStart(lua_State *luaState);

    int onStop(lua_State *luaState);

    int onAllTrade(lua_State *L);

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

private:
    bool isConnectorRunning = false;
    queue<TradeDto> trades;
    recursive_mutex *mutexLock;
    mutex allTradeLock;
    mutex quoteLock;
    mutex changedQuoteMapLock;
    thread checkQuotesThread;
    unordered_map<string, string> changedQuotes;
    QuikCandleService *quikCandleService;
    QueueService *queueService;

    void startCheckQuotesThread();
};

#endif //QUIK_CONNECTOR_QUIK_H
