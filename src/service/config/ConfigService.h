//
// Created by Sergey on 17.07.2021.
//

#ifndef QUIK_CONNECTOR_CONFIGSERVICE_H
#define QUIK_CONNECTOR_CONFIGSERVICE_H

#include <string>
#include "yaml-cpp/yaml.h"
#include "../../dto/config/Config.h"
#include "../../dto/option/Option.h"

using namespace std;

class ConfigService {
public:
    ConfigService(const string& configPath);

    ~ConfigService();

    ConfigDto& getConfig();

private:
    YAML::Node configYaml;
    ConfigDto config;

    Option<string> loadCommonConfig();

    Option<string> loadLogConfig();

    Option<string> loadRedisConfig();

    Option<string> loadOrderConfig();
};

#endif //QUIK_CONNECTOR_CONFIGSERVICE_H
