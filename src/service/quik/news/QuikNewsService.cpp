//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikNewsService.h"

#include <utility>

QuikNewsService::QuikNewsService(const string& newsFilePath) {
    this->newsFilePath = newsFilePath;
    this->fileWatcher = new FileWatcher();
}

QuikNewsService::~QuikNewsService() {
    fileWatcher->removeWatch(watchId);

    delete fileWatcher;
}

/*void QuikNewsService::handleFileAction(WatchID currentWatchId,
                                       const string& dir,
                                       const string& filename,
                                       Action action,
                                       string oldFilename) {
    if (action != Actions::Modified) {
        logger->info("QUIK news file action: {}", action);
        return;
    }
    logger->info("QUIK news file updated....");

    string newsFileData = FileUtils::readFile(newsFilePath);

    onUpdateCallback(newsFileData);
}*/

void QuikNewsService::startMonitorUpdates(UpdateNewsFileCallback callback) {
    if (!configService->getConfig().quik.news.monitorUpdatesEnabled) {
        logger->info("Monitor news updates disabled. Skipping....");
        return;
    }
    logger->info("Start monitor QUIK news file updates with path: {}", newsFilePath);

    this->onUpdateCallback = move(callback);
    this->watchId = fileWatcher->addWatch(newsFilePath, this, false);

    fileWatcher->watch();
}
