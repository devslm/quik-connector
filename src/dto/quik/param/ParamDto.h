//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_PARAMDTO_H
#define QUIK_CONNECTOR_PARAMDTO_H

enum ParamType {
    PARAM_DOUBLE_TYPE,
    PARAM_LONG_TYPE,
    PARAM_CHAR_TYPE,
    PARAM_ENUM_TYPE,
    PARAM_TIME_TYPE,
    PARAM_DATE_TYPE,
    PARAM_UNKNOWN_TYPE
};

typedef struct ParamDto {
    ParamType paramType;
    string paramValue;
    string paramImage;
    string result;
} ParamDto;

#endif //QUIK_CONNECTOR_PARAMDTO_H
