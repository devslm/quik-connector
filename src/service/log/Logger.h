//
// Created by Sergey on 24.06.2021.
//

#ifndef QUIK_CONNECTOR_LOGGER_H
#define QUIK_CONNECTOR_LOGGER_H

#include <string>
#include "../quik/utils/FileUtils.h"
#include "../../dto/config/Config.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

using namespace std;

class Logger {
public:
    static shared_ptr<spdlog::logger> init(const ConfigDto& config);

    static void close();

    static spdlog::level::level_enum Logger::getLogLevelByType(const string& level);
};


#endif //QUIK_CONNECTOR_LOGGER_H
