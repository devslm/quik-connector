//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TRADEACCOUNTDTO_H
#define QUIK_CONNECTOR_TRADEACCOUNTDTO_H

#include <string>
#include <set>

using namespace std;

typedef struct TradeAccountDto {
    set<string> classCodes;
    string firmId;
    string trdAccId;
    string description;
    uint64_t fullCoveredSell;
    string mainTrdAccId;
    string bankIdT0;
    string bankIdTplus;
    double trdAccType = 0.0;
    string depUnitId;
    uint64_t status;
    uint64_t firmUse;
    string depAccId;
    string bankAccId;
} TradeAccountDto;

#endif //QUIK_CONNECTOR_TRADEACCOUNTDTO_H
