//
// Created by Sergey on 16.07.2021.
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
