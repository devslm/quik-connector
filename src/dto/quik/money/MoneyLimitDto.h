//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_MONEYLIMITDTO_H
#define QUIK_CONNECTOR_MONEYLIMITDTO_H

typedef struct MoneyLimitDto {
    double moneyOpenLimit = 0.0;
    double moneyLimitLockedNonMarginalValue = 0.0;
    double moneyLimitLocked = 0.0;
    double moneyOpenBalance = 0.0;
    double moneyCurrentLimit = 0.0;
    double moneyCurrentBalance = 0.0;
    double moneyLimitAvailable = 0.0;
} MoneyLimitDto;

#endif //QUIK_CONNECTOR_MONEYLIMITDTO_H
