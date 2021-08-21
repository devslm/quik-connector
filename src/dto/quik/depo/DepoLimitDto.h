//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DEPOLIMITDTO_H
#define QUIK_CONNECTOR_DEPOLIMITDTO_H

#include "string"

using namespace std;

typedef struct DepoLimitDto {
    string ticker;
    string trdAccId;
    string firmId;
    string clientCode;
    double openBal = 0.0;
    double openLimit = 0.0;
    double currentBal = 0.0;
    double currentLimit = 0.0;
    double lockedSell = 0.0;
    double lockedBuy = 0.0;
    double lockedBuyValue = 0.0;
    double lockedSellValue = 0.0;
    double awgPositionPrice = 0.0;
    uint64_t limitKind;
} DepoLimitDto;

#endif //QUIK_CONNECTOR_DEPOLIMITDTO_H
