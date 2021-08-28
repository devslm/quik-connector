//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_FUTURELIMITDTO_H
#define QUIK_CONNECTOR_FUTURELIMITDTO_H

typedef struct FutureLimitDto {
    string firmId;
    string trdAccId;
    uint64_t limitType;
    double liquidityCoef = 0.0;
    double cbpPrevLimit = 0.0;
    double cbpLimit = 0.0;
    double cbplUsed = 0.0;
    double cbplPlanned = 0.0;
    double varMargin = 0.0;
    double accruedInt = 0.0;
    double cbplUsedForOrders = 0.0;
    double cbplUsedForPositions = 0.0;
    double optionsPremium = 0.0;
    double tsComission = 0.0;
    double kgo = 0.0;
    string currencyCode;
    double realVarMargin = 0.0;
} FutureLimitDto;

#endif //QUIK_CONNECTOR_FUTURELIMITDTO_H
