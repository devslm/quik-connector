//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CLASSINFODTO_H
#define QUIK_CONNECTOR_CLASSINFODTO_H

#include <string>

typedef struct ClassInfoDto {
    std::string firmId;
    std::string name;
    std::string code;
    double npars;
    double nsecs;
} ClassInfoDto;

#endif //QUIK_CONNECTOR_CLASSINFODTO_H
