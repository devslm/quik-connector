//
// Created by Sergey on 20.07.2021.
//

#ifndef QUIK_CONNECTOR_ORDERDTO_H
#define QUIK_CONNECTOR_ORDERDTO_H

#include <string>

using namespace std;

typedef struct OrderDto {
    uint64_t orderNum;
    double flags;
    string brokerRef;
    string userId;
    string firmId;
    string account;
    double price;
    double qty;
    double balance;
    double value;
    double accruedInt;
    double yield;
    uint64_t transId;
    string clientCode;
    double price2;
    string settleCode;
    uint64_t uid;
    uint64_t canceledUid;
    string exchangeCode;
    double activationTime;
    uint64_t linkedOrder;
    double expiry;
    string ticker;
    string name;
    string classCode;
    string classType;
    string status;
    string type;
    string currency;
    double lotSize;
    uint64_t date;
    uint64_t withdrawDate;
    string bankAccId;
    uint64_t valueEntryType;
    double repoTerm;
    double repoValue;
    double repo2value;
    double repoValueBalance;
    double startDiscount;
    string rejectReason;
    double extOrderFlags;
    double minQty;
    uint64_t execType;
    double sideQualifier;
    double acntType;
    double capacity;
    double passiveOnlyOrder;
    double visible;
    OrderCommissionDto commission;
} OrderDto;

#endif //QUIK_CONNECTOR_ORDERDTO_H
