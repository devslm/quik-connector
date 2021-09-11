//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikNewsService.h"

QuikNewsService::QuikNewsService(Quik *quik) {
    this->quik = quik;
}

QuikNewsService::~QuikNewsService() {
    monitorNewsThread.join();
}

void QuikNewsService::startMonitorUpdates(lua_State *luaState) {
    if (!configService->getConfig().quik.news.monitorUpdatesEnabled) {
        logger->info("Monitor news updates disabled. Skipping....");
        return;
    }
    monitorNewsThread = thread([&luaState, this]() {
        auto quikPath = quik->getWorkingFolder(luaState);

        if (quikPath.isEmpty()) {
            logger->error("Could not start QUIK news file monitor updates because QUIK working folder is invalid!");
            return;
        }
        auto newsFilePath = quikPath.get() + "\\" + configService->getConfig().quik.news.fileName;

        logger->info("Start monitor QUIK news file updates with path: {}", newsFilePath);

        while (quik->isRunning()) {
            this_thread::sleep_for(chrono::seconds(5));

            // TODO Add monitor news file update
        }
    });
}
