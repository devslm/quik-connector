//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_REQUESTDTO_H
#define QUIK_CONNECTOR_REQUESTDTO_H

#include <string>
#include "../../../../service/quik/utils/QuikUtils.h"

using namespace std;

typedef struct RequestDto {
    string id;
    string command;
} RequestDto;

typedef struct TickerLastPriceDto {
    string ticker;
    string classCode;
} TickerLastPriceDto;

typedef struct NewStopOrderRequestDto {
    string account;
    string clientCode;
    string ticker;
    string classCode;
    string operation;
    int quantity;
    double stopPrice;
    double price;
    string expiryDate;
} NewStopOrderRequestDto;

typedef struct CancelStopOrderRequestDto {
    uint64_t stopOrderId;
    string account;
    string clientCode;
    string ticker;
    string classCode;
} CancelStopOrderRequestDto;

typedef struct SellByMarketRequestDto {
    uint64_t transactionId;
    string account;
    string clientCode;
    string ticker;
    string classCode;
    int lots;
} SellByMarketRequestDto;

typedef struct SubscribeToCandlesRequestDto {
    string ticker;
    string classCode;
    Interval interval;
} SubscribeToCandlesRequestDto;

typedef struct UnsubscribeFromCandlesRequestDto {
    string ticker;
    string classCode;
    Interval interval;
} UnsubscribeFromCandlesRequestDto;

typedef struct CandlesRequestDto {
    string ticker;
    string classCode;
    Interval interval;
} CandlesRequestDto;

#endif //QUIK_CONNECTOR_REQUESTDTO_H
