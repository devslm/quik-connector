//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TICKERQUOTEDTO_H
#define QUIK_CONNECTOR_TICKERQUOTEDTO_H

typedef struct TickerPriceDto {
    double price = 0.0;
    int quantity;
} TickerPriceDto;

typedef struct TickerQuoteDto {
    string classCode;
    string ticker;
    int bidCount;
    int offerCount;
    list<TickerPriceDto> bids;
    list<TickerPriceDto> offers;
} TickerQuoteDto;

#endif //QUIK_CONNECTOR_TICKERQUOTEDTO_H
