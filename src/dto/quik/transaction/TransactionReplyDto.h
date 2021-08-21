//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TRANSACTIONREPLYDTO_H
#define QUIK_CONNECTOR_TRANSACTIONREPLYDTO_H

#include "string"

using namespace std;

typedef struct TransactionReplyDto {
    uint64_t transId;
    uint64_t status;
    string statusCode;
    string resultMsg;
    uint64_t date;
    uint64_t uid;
    double flags = 0.0;
    uint64_t serverTransId;
    uint64_t orderNum;
    double price = 0.0;
    double quantity = 0.0;
    double balance = 0.0;
    string firmId;
    string account;
    string clientCode;
    string brokerRef;
    string classCode;
    string ticker;
    string exchangeCode;
} TransactionReplyDto;

#endif //QUIK_CONNECTOR_TRANSACTIONREPLYDTO_H
