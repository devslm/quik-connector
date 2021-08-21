//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TICKERDTO_H
#define QUIK_CONNECTOR_TICKERDTO_H

#include <string>

using namespace std;

typedef struct TickerDto {
    string code;
    string name;
    string shortName;
    string classCode;
    string className;
    double faceValue;
    string faceUnit;
    double scale;
    uint64_t matDate;
    double lotSize;
    string isinCode;
    double minPriceStep;
} TickerDto;

#endif //QUIK_CONNECTOR_TICKERDTO_H
