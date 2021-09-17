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
    QuikNewsService(const string& newsDirectory, const string& newsFileName);

    virtual ~QuikNewsService();

    void startMonitorUpdates(UpdateNewsFileCallback callback);

    void handleFileAction(WatchID currentWatchId,
                          const std::string& dir,
                          const std::string& filename,
                          Action action,
                          std::string oldFilename) override;

private:
    string newsDirectory;
    string newsFileName;
    UpdateNewsFileCallback onUpdateCallback;
    FileWatcher* fileWatcher;
    WatchID watchId;
};

#endif //QUIK_CONNECTOR_NEWSSERVICE_H
