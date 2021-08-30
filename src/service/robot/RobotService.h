//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ROBOTSERVICE_H
#define QUIK_CONNECTOR_ROBOTSERVICE_H

#include "../quik/Quik.h"

class Quik;

class RobotService {
public:
    explicit RobotService(Quik* quik);

    virtual ~RobotService();

    void run(lua_State* luaState);

private:
    Quik* quik;
    thread robotInternalThread;

    void runQuikConnectionStatusMonitor(lua_State* luaState);
};

#endif //QUIK_CONNECTOR_ROBOTSERVICE_H
