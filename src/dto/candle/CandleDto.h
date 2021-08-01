//
// Created by Sergey on 01.07.2021.
//

#ifndef QUIK_CONNECTOR_CANDLEDTO_H
#define QUIK_CONNECTOR_CANDLEDTO_H

#include <string>

using namespace std;

typedef struct CandleValueDto {
    double open = 0.0;
    double close = 0.0;
    double high = 0.0;
    double low = 0.0;
    double volume = 0.0;
    uint64_t date = 0;
} CandleValueDto;

typedef struct CandleDto {
    string classCode;
    string ticker;
    string interval;
    int candleLastIndex;
    list<CandleValueDto> values;
} CandleDto;

#endif //QUIK_CONNECTOR_CANDLEDTO_H
