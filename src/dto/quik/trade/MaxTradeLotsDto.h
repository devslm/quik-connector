//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_MAXTRADELOTSDTO_H
#define QUIK_CONNECTOR_MAXTRADELOTSDTO_H

#include "string"

using namespace std;

typedef struct MaxTradeLotsDto {
    double qty = 0.0;
    double commission = 0.0;
} MaxTradeLotsDto;

#endif //QUIK_CONNECTOR_MAXTRADELOTSDTO_H
