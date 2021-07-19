//
// Created by Sergey on 24.06.2021.
//

#include "Logger.h"

Logger::Logger(const ConfigDto& config) {
    auto filePath = config.log.path + config.directorySeparator + config.log.name;
    logLevel = getLogLevelByType(config.log.level);
    logFile = fopen(filePath.c_str(), "w");

    log_set_level(logLevel);
    log_add_fp(logFile, logLevel);
}

Logger::~Logger() {
    if (logFile != nullptr) {
        fclose(logFile);
    }
}

void Logger::info(const char *fmt...) {
    log_log(LOG_INFO,  __FILE__, __LINE__, fmt);
}

int Logger::getLogLevelByType(const string& level) {
    if ("DEBUG" == level) {
        return LOG_DEBUG;
    } else if ("INFO" == level) {
        return LOG_INFO;
    } else if ("WARN" == level) {
        return LOG_WARN;
    } else if ("ERROR" == level) {
        return LOG_ERROR;
    }
    return LOG_INFO;
}
