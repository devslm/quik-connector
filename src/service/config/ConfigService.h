//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CONFIGSERVICE_H
#define QUIK_CONNECTOR_CONFIGSERVICE_H

#include <string>
#include "yaml-cpp/yaml.h"
#include "../../dto/config/Config.h"
#include "../../dto/option/Option.h"
#include "../utils/string/StringUtils.h"

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

    Option<string> loadDbConfig();

    Option<string> loadQuikConfig();

    Option<string> loadDebugConfig();
};

#endif //QUIK_CONNECTOR_CONFIGSERVICE_H
