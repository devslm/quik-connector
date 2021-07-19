//
// Created by Sergey on 24.06.2021.
//

#ifndef QUIK_CONNECTOR_LOGGER_H
#define QUIK_CONNECTOR_LOGGER_H

#include <stdio.h>
#include <string>
#include "../../dto/config/Config.h"

extern "C" {
#include "log.h"
};

using namespace std;

class Logger {
public:
    Logger(const ConfigDto& config);

    void info(const char *fmt...);

    ~Logger();

private:
    int logLevel;
    FILE *logFile;

    int getLogLevelByType(const string& logLevel);
};


#endif //QUIK_CONNECTOR_LOGGER_H
