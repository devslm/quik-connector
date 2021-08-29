//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_NEWSSERVICE_H
#define QUIK_CONNECTOR_NEWSSERVICE_H

#include <string>
#include "../../quik/Quik.h"

using namespace std;

class Quik;
class ConfigService;

class QuikNewsService {
public:
    explicit QuikNewsService(Quik *quik);

    virtual ~QuikNewsService();

    void startMonitorUpdates(lua_State *luaState);

private:
    Quik *quik;
    thread monitorNewsThread;
};

#endif //QUIK_CONNECTOR_NEWSSERVICE_H
