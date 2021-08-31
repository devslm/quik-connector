//
// Created by Sergey on 24.06.2021.
//

#include "Logger.h"

const auto QUIK_CONNECTOR_LOGGER = "quik-connector";

shared_ptr<spdlog::logger> Logger::init(const ConfigDto& config) {
    auto logger = spdlog::rotating_logger_mt(
        QUIK_CONNECTOR_LOGGER,
        config.log.path + config.directorySeparator + config.log.name,
        config.log.fileMaxSize * 1024 * 1024,
        config.log.maxFiles
    );
    auto logLevel = getLogLevelByType(config.log.level);

    logger->set_level(logLevel);
    logger->flush_on(logLevel);

    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%-7l] [%-5t]  %v");

    return logger;
}

spdlog::level::level_enum Logger::getLogLevelByType(const string& level) {
    if ("DEBUG" == level) {
        return spdlog::level::debug;
    } else if ("INFO" == level) {
        return spdlog::level::info;
    } else if ("WARN" == level) {
        return spdlog::level::warn;
    } else if ("ERROR" == level) {
        return spdlog::level::err;
    }
    return spdlog::level::info;
}
