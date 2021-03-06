//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CHANGEDCANDLEDTO_H
#define QUIK_CONNECTOR_CHANGEDCANDLEDTO_H

#include "CandleDto.h"

typedef struct ChangedCandleDto {
    string classCode;
    string ticker;
    string interval;
    CandleValueDto previousCandle;
    CandleValueDto currentCandle;
} ChangedCandleDto;

#endif //QUIK_CONNECTOR_CHANGEDCANDLEDTO_H
