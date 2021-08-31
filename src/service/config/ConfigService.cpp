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
    return {};
}

Option<string> ConfigService::loadLogConfig() {
    if (!configYaml["log"]) {
        return {"Log section required"};
    } else if (!configYaml["log"]["level"]) {
        return {"Log level required"};
    } else if (!configYaml["log"]["path"]) {
        return {"Log path required"};
    } else if (!configYaml["log"]["name"]) {
        return {"Log name required"};
    } else if (!configYaml["log"]["max"]) {
        return {"Log max section required"};
    } else if (!configYaml["log"]["max"]["size"]) {
        return {"Log max size required"};
    } else if (!configYaml["log"]["max"]["files"]) {
        return {"Log max files required"};
    }
    config.log.level = configYaml["log"]["level"].as<string>();
    config.log.level = stringTrim(config.log.level);

    config.log.path = config.scriptPath + "\\" + configYaml["log"]["path"].as<string>();
    config.log.path = stringTrim(config.log.path);

    config.log.name = configYaml["log"]["name"].as<string>();
    config.log.name = stringTrim(config.log.name);

    config.log.fileMaxSize = configYaml["log"]["max"]["size"].as<int>();
    config.log.maxFiles = configYaml["log"]["max"]["files"].as<int>();

    return {};
}

Option<string> ConfigService::loadRedisConfig() {
    if (!configYaml["redis"]) {
        return {"Redis section required"};
    } else if (!configYaml["redis"]["enabled"]) {
        return {"Redis enabled required"};
    } else if (!configYaml["redis"]["host"]) {
        return {"Redis host required"};
    } else if (!configYaml["redis"]["port"]) {
        return {"Redis port required"};
    } else if (!configYaml["redis"]["password"]) {
        return {"Redis password required"};
    }
    config.redis.isEnabled = configYaml["redis"]["enabled"].as<bool>();

    config.redis.host = configYaml["redis"]["host"].as<string>();
    config.redis.host = stringTrim(config.redis.host);

    config.redis.port = configYaml["redis"]["port"].as<int>();

    if (!configYaml["redis"]["password"].as<string>().empty()) {
        string redisPassword = configYaml["redis"]["password"].as<string>();
        config.redis.password = {stringTrim(redisPassword)};
    } else {
        config.redis.password = {};
    }
    return {};
}

Option<string> ConfigService::loadDbConfig() {
    if (!configYaml["db"]) {
        return {"Db section required"};
    } else if (!configYaml["db"]["path"]) {
        return {"Db path required"};
    } else if (!configYaml["db"]["name"]) {
        return {"Db name required"};
    } else if (!configYaml["db"]["migrations"]["path"]) {
        return {"Db migrations path required"};
    }
    config.db.path = configYaml["db"]["path"].as<string>();
    config.db.path = stringTrim(config.db.path);

    config.db.name = configYaml["db"]["name"].as<string>();
    config.db.name = stringTrim(config.db.name);

    config.db.migrationsPath = configYaml["db"]["migrations"]["path"].as<string>();
    config.db.migrationsPath = stringTrim(config.db.migrationsPath);

    return {};
}

Option<string> ConfigService::loadQuikConfig() {
    if (!configYaml["quik"]) {
        return {"Quik section required"};
    } else if (!configYaml["quik"]["callback"]) {
        return {"Quik callback required"};
    } else if (!configYaml["quik"]["callback"]["enabled"]) {
        return {"Quik callback enabled required"};
    } else if (!configYaml["quik"]["callback"]["enabled"]["all-trade"]) {
        return {"Quik callback enabled all-trade required"};
    } else if (!configYaml["quik"]["callback"]["enabled"]["quote"]) {
        return {"Quik callback enabled quote required"};
    } else if (!configYaml["quik"]["callback"]["enabled"]["order"]) {
        return {"Quik callback enabled order required"};
    } else if (!configYaml["quik"]["callback"]["enabled"]["trans-reply"]) {
        return {"Quik callback enabled trans-reply required"};
    }
    config.quik.callback.onAllTradeEnabled = configYaml["quik"]["callback"]["enabled"]["all-trade"].as<bool>();
    config.quik.callback.onQuoteEnabled = configYaml["quik"]["callback"]["enabled"]["quote"].as<bool>();
    config.quik.callback.onOrderEnabled = configYaml["quik"]["callback"]["enabled"]["order"].as<bool>();
    config.quik.callback.onTransReplyEnabled = configYaml["quik"]["callback"]["enabled"]["trans-reply"].as<bool>();

    if (!configYaml["quik"]["order"]) {
        return {"Quik order section required"};
    } else if (!configYaml["quik"]["order"]["ignore"]) {
        return {"Order ignore required"};
    } else if (!configYaml["quik"]["order"]["ignore"]["canceled"]) {
        return {"Order ignore canceled required"};
    } else if (!configYaml["quik"]["order"]["save"]) {
        return {"Order save required"};
    }
    config.quik.order.ignoreCancelled = configYaml["quik"]["order"]["ignore"]["canceled"].as<bool>();
    config.quik.order.saveToDb = configYaml["quik"]["order"]["save"].as<bool>();

    if (!configYaml["quik"]["news"]) {
        return {"Quik news section required"};
    } else if (!configYaml["quik"]["news"]["file"]) {
        return {"News file required"};
    } else if (!configYaml["quik"]["news"]["file"]["name"]) {
        return {"News file name required"};
    } else if (!configYaml["quik"]["news"]["monitor"]) {
        return {"News monitor required"};
    } else if (!configYaml["quik"]["news"]["monitor"]["enabled"]) {
        return {"News monitor enabled required"};
    }
    config.quik.news.fileName = configYaml["quik"]["news"]["file"]["name"].as<string>();
    config.quik.news.fileName = stringTrim(config.quik.news.fileName);

    config.quik.news.monitorUpdatesEnabled = configYaml["quik"]["news"]["monitor"]["enabled"].as<bool>();
    
    return {};
};

Option<string> ConfigService::loadDebugConfig() {
    if (!configYaml["debug"]) {
        return {"Debug section required"};
    } else if (!configYaml["debug"]["print"]) {
        return {"Debug print required"};
    } else if (!configYaml["debug"]["print"]["lua-stack"]) {
        return {"Debug print lua-stack required"};
    }
    config.debug.printLuaStack = configYaml["debug"]["print"]["lua-stack"].as<bool>();

    return {};
}

ConfigDto& ConfigService::getConfig() {
    return config;
}
