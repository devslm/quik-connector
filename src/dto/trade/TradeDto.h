//
// Created by Sergey on 24.06.2021.
//

#ifndef QUIK_CONNECTOR_TRADEDTO_H
#define QUIK_CONNECTOR_TRADEDTO_H

#include <string>

using namespace std;

typedef struct TradeDto {
    uint64_t tradeNum;
    uint64_t orderNum;
    double flags;
    double price;
    double qty;
    double value;
    double accruedInt;
    double yield;
    string settleCode;
    double repoRate;
    double repoValue;
    double repo2value;
    double repoTerm;
    string secCode;
    string classCode;
    uint64_t date;
    double period;
    string exchangeCode;
    double clearingComission;
    double exchangeComission;
    double techCenterComission;
    double brokerComission;
} TradeDto;

#endif //QUIK_CONNECTOR_TRADEDTO_H
