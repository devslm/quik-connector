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
    double flags = 0.0;
    double price = 0.0;
    double qty = 0.0;
    double value = 0.0;
    double accruedInt = 0.0;
    double yield = 0.0;
    string settleCode;
    double repoRate = 0.0;
    double repoValue = 0.0;
    double repo2value = 0.0;
    double repoTerm = 0.0;
    string ticker;
    string classCode;
    uint64_t date;
    double period = 0.0;
    string exchangeCode;
    double clearingComission = 0.0;
    double exchangeComission = 0.0;
    double techCenterComission = 0.0;
    double brokerComission = 0.0;
} TradeDto;

#endif //QUIK_CONNECTOR_TRADEDTO_H
