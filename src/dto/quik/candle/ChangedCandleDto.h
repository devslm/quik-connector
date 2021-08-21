//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CHANGEDCANDLEDTO_H
#define QUIK_CONNECTOR_CHANGEDCANDLEDTO_H

typedef struct ChangedCandleDto {
    string classCode;
    string ticker;
    string interval;
    CandleValueDto previousCandle;
    CandleValueDto currentCandle;
} ChangedCandleDto;

#endif //QUIK_CONNECTOR_CHANGEDCANDLEDTO_H
