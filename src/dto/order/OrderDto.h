//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERDTO_H
#define QUIK_CONNECTOR_ORDERDTO_H

#include <string>
#include "OrderCommissionDto.h"

using namespace std;

typedef struct OrderDto {
    uint64_t orderNum;
    double flags = 0.0;
    string brokerRef;
    string userId;
    string firmId;
    string account;
    double price = 0.0;
    double qty = 0.0;
    double balance = 0.0;
    double value = 0.0;
    double accruedInt = 0.0;
    double yield = 0.0;
    uint64_t transId;
    string clientCode;
    double price2 = 0.0;
    string settleCode;
    uint64_t uid;
    uint64_t canceledUid;
    string exchangeCode;
    double activationTime = 0.0;
    uint64_t linkedOrder;
    double expiry = 0.0;
    string ticker;
    string name;
    string classCode;
    string classType;
    string status;
    string type;
    string currency;
    double lotSize = 0.0;
    uint64_t date;
    uint64_t withdrawDate;
    string bankAccId;
    uint64_t valueEntryType;
    double repoTerm = 0.0;
    double repoValue = 0.0;
    double repo2value = 0.0;
    double repoValueBalance = 0.0;
    double startDiscount = 0.0;
    string rejectReason;
    double extOrderFlags = 0.0;
    double minQty = 0.0;
    uint64_t execType = 0.0;
    double sideQualifier = 0.0;
    double acntType = 0.0;
    double capacity = 0.0;
    double passiveOnlyOrder = 0.0;
    double visible = 0.0;
    double priceStepCost = 0.0;
    OrderCommissionDto commission;
} OrderDto;

#endif //QUIK_CONNECTOR_ORDERDTO_H
