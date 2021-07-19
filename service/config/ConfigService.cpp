//
// Created by Sergey on 17.07.2021.
//

#include "ConfigService.h"

ConfigService::ConfigService(const string& scriptPath) {
    auto configPath = scriptPath + "\\config.yml";
    this->configYaml = YAML::LoadFile(configPath);
    config.scriptPath = scriptPath;
    list<Option<string>> errorList;
    string errorDetails;

    errorList.push_back(loadCommonConfig());
    errorList.push_back(loadLogConfig());
    errorList.push_back(loadRedisConfig());

    for (Option<string>& error : errorList) {
        if (error.isPresent()) {
            errorDetails += " - " + error.get() + "\n";
        }
    }

    if (!errorDetails.empty()) {
        throw runtime_error("Could not load app config! Reasons: \n" + errorDetails);
    }
}

ConfigService::~ConfigService() {

}

Option<string> ConfigService::loadCommonConfig() {
    if (!configYaml["version"]) {
        return Option<string>("Version required");
    }
    config.version = configYaml["version"].as<string>();

    return Option<string>();
}

Option<string> ConfigService::loadLogConfig() {
    if (!configYaml["log"]) {
        return Option<string>("Log section required");
    } else if (!configYaml["log"]["level"]) {
        return Option<string>("Log level required");
    } else if (!configYaml["log"]["path"]) {
        return Option<string>("Log path required");
    } else if (!configYaml["log"]["name"]) {
        return Option<string>("Log name required");
    }
    config.log.level = configYaml["log"]["level"].as<string>();
    config.log.path = config.scriptPath + "\\" + configYaml["log"]["path"].as<string>();
    config.log.name = configYaml["log"]["name"].as<string>();

    return Option<string>();
}

Option<string> ConfigService::loadRedisConfig() {
    if (!configYaml["redis"]) {
        return Option<string>("Redis section required");
    } else if (!configYaml["redis"]["host"]) {
        return Option<string>("Redis host required");
    } else if (!configYaml["redis"]["port"]) {
        return Option<string>("Redis port required");
    }
    config.redis.host = configYaml["redis"]["host"].as<string>();
    config.redis.port = configYaml["redis"]["port"].as<int>();

    return Option<string>();
}

ConfigDto& ConfigService::getConfig() {
    return config;
}
