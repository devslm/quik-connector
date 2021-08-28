//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "ConfigService.h"

ConfigService::ConfigService(const string& scriptPath) {
    auto configPath = scriptPath + "\\config\\config.yml";
    this->configYaml = YAML::LoadFile(configPath);
    config.scriptPath = scriptPath;
    list<Option<string>> errorList;
    string errorDetails;

    errorList.push_back(loadCommonConfig());
    errorList.push_back(loadLogConfig());
    errorList.push_back(loadRedisConfig());
    errorList.push_back(loadDbConfig());
    errorList.push_back(loadQuikConfig());
    errorList.push_back(loadDebugConfig());

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
    } else if (!configYaml["log"]["max"]) {
        return Option<string>("Log max section required");
    } else if (!configYaml["log"]["max"]["size"]) {
        return Option<string>("Log max size required");
    } else if (!configYaml["log"]["max"]["files"]) {
        return Option<string>("Log max files required");
    }
    config.log.level = configYaml["log"]["level"].as<string>();
    config.log.path = config.scriptPath + "\\" + configYaml["log"]["path"].as<string>();
    config.log.name = configYaml["log"]["name"].as<string>();
    config.log.fileMaxSize = configYaml["log"]["max"]["size"].as<int>();
    config.log.maxFiles = configYaml["log"]["max"]["files"].as<int>();

    return Option<string>();
}

Option<string> ConfigService::loadRedisConfig() {
    if (!configYaml["redis"]) {
        return Option<string>("Redis section required");
    } else if (!configYaml["redis"]["host"]) {
        return Option<string>("Redis host required");
    } else if (!configYaml["redis"]["port"]) {
        return Option<string>("Redis port required");
    } else if (!configYaml["redis"]["password"]) {
        return Option<string>("Redis password required");
    }
    config.redis.host = configYaml["redis"]["host"].as<string>();
    config.redis.port = configYaml["redis"]["port"].as<int>();

    if (!configYaml["redis"]["password"].as<string>().empty()) {
        config.redis.password = Option<string>(configYaml["redis"]["password"].as<string>());
    } else {
        config.redis.password = Option<string>();
    }
    return Option<string>();
}

Option<string> ConfigService::loadDbConfig() {
    if (!configYaml["db"]) {
        return Option<string>("Db section required");
    } else if (!configYaml["db"]["path"]) {
        return Option<string>("Db path required");
    } else if (!configYaml["db"]["name"]) {
        return Option<string>("Db name required");
    } else if (!configYaml["db"]["migrations"]["path"]) {
        return Option<string>("Db migrations path required");
    }
    config.db.path = configYaml["db"]["path"].as<string>();
    config.db.name = configYaml["db"]["name"].as<string>();
    config.db.migrationsPath = configYaml["db"]["migrations"]["path"].as<string>();

    return Option<string>();
}

Option<string> ConfigService::loadQuikConfig() {
    if (!configYaml["quik"]) {
        return Option<string>("Quik section required");
    } else if (!configYaml["quik"]["callback"]) {
        return Option<string>("Quik callback required");
    } else if (!configYaml["quik"]["callback"]["enabled"]) {
        return Option<string>("Quik callback enabled required");
    } else if (!configYaml["quik"]["callback"]["enabled"]["all-trade"]) {
        return Option<string>("Quik callback enabled all-trade required");
    } else if (!configYaml["quik"]["callback"]["enabled"]["quote"]) {
        return Option<string>("Quik callback enabled quote required");
    } else if (!configYaml["quik"]["callback"]["enabled"]["order"]) {
        return Option<string>("Quik callback enabled order required");
    } else if (!configYaml["quik"]["callback"]["enabled"]["trans-reply"]) {
        return Option<string>("Quik callback enabled trans-reply required");
    }
    config.quik.callback.onAllTradeEnabled = configYaml["quik"]["callback"]["enabled"]["all-trade"].as<bool>();
    config.quik.callback.onQuoteEnabled = configYaml["quik"]["callback"]["enabled"]["quote"].as<bool>();
    config.quik.callback.onOrderEnabled = configYaml["quik"]["callback"]["enabled"]["order"].as<bool>();
    config.quik.callback.onTransReplyEnabled = configYaml["quik"]["callback"]["enabled"]["trans-reply"].as<bool>();

    if (!configYaml["quik"]["order"]) {
        return Option<string>("Quik order section required");
    } else if (!configYaml["quik"]["order"]["ignore"]) {
        return Option<string>("Order ignore required");
    } else if (!configYaml["quik"]["order"]["ignore"]["canceled"]) {
        return Option<string>("Order ignore canceled required");
    } else if (!configYaml["quik"]["order"]["save"]) {
        return Option<string>("Order save required");
    }
    config.quik.order.ignoreCancelled = configYaml["quik"]["order"]["ignore"]["canceled"].as<bool>();
    config.quik.order.saveToDb = configYaml["quik"]["order"]["save"].as<bool>();

    return Option<string>();
};

Option<string> ConfigService::loadDebugConfig() {
    if (!configYaml["debug"]) {
        return Option<string>("Debug section required");
    } else if (!configYaml["debug"]["print"]) {
        return Option<string>("Debug print required");
    } else if (!configYaml["debug"]["print"]["lua-stack"]) {
        return Option<string>("Debug print lua-stack required");
    }
    config.debug.printLuaStack = configYaml["debug"]["print"]["lua-stack"].as<bool>();

    return Option<string>();
}

ConfigDto& ConfigService::getConfig() {
    return config;
}
