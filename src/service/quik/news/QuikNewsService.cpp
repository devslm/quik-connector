//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "QuikNewsService.h"

#include <utility>

QuikNewsService::QuikNewsService(const string& newsDirectory, const string& newsFileName) {
    this->newsDirectory = newsDirectory;
    this->newsFileName = newsFileName;
    this->fileWatcher = new FileWatcher();
}

QuikNewsService::~QuikNewsService() {
    fileWatcher->removeWatch(watchId);

    delete fileWatcher;
}

void QuikNewsService::handleFileAction(WatchID currentWatchId,
                                       const string& dir,
                                       const string& filename,
                                       Action action,
                                       string oldFilename) {
    if (filename != newsFileName
            || action != Actions::Modified) {
        return;
    }
    logger->debug("QUIK news file updated....");

    string newsFilePath = newsDirectory + "\\" + newsFileName;
    string newsFileData = FileUtils::readFile(newsFilePath);

    onUpdateCallback(newsFileData);
}

void QuikNewsService::startMonitorUpdates(UpdateNewsFileCallback callback) {
    if (!configService->getConfig().quik.news.monitorUpdatesEnabled) {
        logger->info("Monitor news updates disabled. Skipping....");
        return;
    }
    this->onUpdateCallback = move(callback);
    this->watchId = fileWatcher->addWatch(newsDirectory, this, true);
    string newsFilePath = newsDirectory + "\\" + newsFileName;

    switch (watchId) {
        case efsw::Errors::FileNotFound:
        case efsw::Errors::FileRepeated:
        case efsw::Errors::FileOutOfScope:
        case efsw::Errors::FileRemote:
        case efsw::Errors::Unspecified:
            logger->warn("Could not start monitor QUIK news file updates with path: {}! Reason: {}",
                newsFilePath, Errors::Log::getLastErrorLog());
            break;
        default:
            logger->info("Start monitor QUIK news file updates....");

            fileWatcher->watch();
    }
}
