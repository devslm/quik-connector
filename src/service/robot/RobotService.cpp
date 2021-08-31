//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "RobotService.h"

RobotService::RobotService(Quik* quik) {
    this->quik = quik;
}

RobotService::~RobotService() {
    // Don't forget to join any internal threads otherwise DLL will crash
    robotInternalThread.join();
}

void RobotService::run(lua_State* luaState) {
    // Implement robot custom logic here
    // Current logic just for example

    runQuikConnectionStatusMonitor(luaState);

    logger->info("[Robot] Finished main thread");
}

void RobotService::runQuikConnectionStatusMonitor(lua_State* luaState) {
    robotInternalThread = thread([this, luaState]() {
        int totalLoops = 20;

        while (quik->isRunning()) {
            // Avoid sleep for a long time because we can't exit properly due to QUIK timeout
            this_thread::sleep_for(chrono::milliseconds(500));

            --totalLoops;

            if (totalLoops >= 0) {
                continue;
            }
            totalLoops = 20;
            auto connectionStatus = quik->getServerConnectionStatus(luaState);

            if (connectionStatus.isPresent()) {
                if (connectionStatus.get().isConnected) {
                    logger->info("[Robot] QUIK server time: {} (connection status: connected)",
                        quik->getServerTime(luaState).get());
                } else {
                    logger->info("[Robot] QUIK (connection status: disconnected)");
                }
            } else {
                logger->error("[Robot] Could not get QUIK connection information!");
            }
        }
    });
}
