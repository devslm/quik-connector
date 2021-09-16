//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_NEWSSERVICE_H
#define QUIK_CONNECTOR_NEWSSERVICE_H

#include <string>
#include <efsw/efsw.hpp>
#include "../../lua/Lua.h"

using namespace std;
using namespace efsw;

class Quik;

typedef function<void(string&)> UpdateNewsFileCallback;

class QuikNewsService : public FileWatchListener {
public:
    explicit QuikNewsService(const string& newsFilePath);

    virtual ~QuikNewsService();

    void startMonitorUpdates(UpdateNewsFileCallback callback);

    void handleFileAction(WatchID watchid, const std::string& dir, const std::string& filename, Action action, std::string oldFilename) override {
        if (action != Actions::Modified) {
            logger->info("QUIK news file action: {}", action);
            return;
        }
        logger->info("QUIK news file updated....");

        string newsFileData = FileUtils::readFile(newsFilePath);

        onUpdateCallback(newsFileData);
    }

private:
    string newsFilePath;
    UpdateNewsFileCallback onUpdateCallback;
    FileWatcher* fileWatcher;
    WatchID watchId;
};

#endif //QUIK_CONNECTOR_NEWSSERVICE_H
