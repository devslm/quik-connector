//
// Created by Sergey on 01.07.2021.
//

#ifndef QUIK_CONNECTOR_CANDLEDTO_H
#define QUIK_CONNECTOR_CANDLEDTO_H

#include <string>

using namespace std;

typedef struct CandleValueDto {
    double open;
    double close;
    double high;
    double low;
    double volume;
    uint64_t date;
} CandleValueDto;

typedef struct CandleDto {
    string classCode;
    string ticker;
    string interval;
    int candleLastIndex;
    list<CandleValueDto> values;
} CandleDto;

#endif //QUIK_CONNECTOR_CANDLEDTO_H
