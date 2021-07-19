//
// Created by Sergey on 24.06.2021.
//

#ifndef QUIK_CONNECTOR_TRADEDTO_H
#define QUIK_CONNECTOR_TRADEDTO_H

#include <string>

typedef struct TradeDto {
    double tradeNum;
    double flags;
    double price;
    double qty;
    double value;
    double accruedint;
    double yield;
    std::string settlecode;
    double reporate;
    double repovalue;
    double repo2value;
    double repoterm;
    std::string secCode;
    std::string classCode;
    uint64_t date;
    double period;
    double openInterest;
    std::string exchangeCode;
} TradeDto;

#endif //QUIK_CONNECTOR_TRADEDTO_H
