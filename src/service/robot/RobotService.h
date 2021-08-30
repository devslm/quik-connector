//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ROBOTSERVICE_H
#define QUIK_CONNECTOR_ROBOTSERVICE_H

#include "../quik/Quik.h"

class Quik;

extern Quik* quik;

class RobotService {
public:
    RobotService() = default;

    void run(lua_State* luaState);
};

#endif //QUIK_CONNECTOR_ROBOTSERVICE_H
