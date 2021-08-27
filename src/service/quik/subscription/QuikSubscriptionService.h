//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_QUIKSUBSCRIPTIONSERVICE_H
#define QUIK_CONNECTOR_QUIKSUBSCRIPTIONSERVICE_H

#include <string>
#include <map>
#include "../Quik.h"
#include "../../../dto/quik/connector/subscription/QuikSubscriptionDto.h"

using namespace std;

class QuikSubscriptionService {
public:
    QuikSubscriptionService();

    virtual ~QuikSubscriptionService();

    void subscribeToImpersonalTransactions(lua_State *luaState, string& classCode, string& ticker);

    void subscribeToCandles(lua_State *luaState, string& classCode, string& ticker, Interval& interval);

private:
    atomic_bool isRunning;
    recursive_mutex *mutexLock;
    map<string, QuikSubscriptionDto> impersonalTransactionSubscriptions;
    map<string, QuikSubscriptionDto> candleSubscriptions;

    Option<QuikSubscriptionDto> createDataSource(lua_State *luaState,
                                                   string& classCode,
                                                   string& ticker,
                                                   Interval& interval);

    bool getCandlesSize(QuikSubscriptionDto *candleSubscription, int *buffer);

    bool isSubscriptionExists(string& classCode, string& ticker, Interval& interval, bool& isImpersonalSubscription);

    string createSubscriptionKey(string& classCode, string& ticker, Interval& interval);

    void addSubscription(string& classCode, string& ticker, Interval& interval, bool& isImpersonalSubscription);
};

#endif //QUIK_CONNECTOR_QUIKSUBSCRIPTIONSERVICE_H
