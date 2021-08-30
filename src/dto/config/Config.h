//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CONFIG_H
#define QUIK_CONNECTOR_CONFIG_H

#ifdef _WIN32
#define DIRECTORY_SEPARATOR "\\"
#else
#define DIRECTORY_SEPARATOR "/"
#endif

#include <string>
#include "../../version.h"
#include "../../dto/option/Option.h"

using namespace std;

const string APP_VERSION = to_string(VERSION_MAJOR).append(".")
    .append(to_string(VERSION_MINOR)).append(".")
    .append(to_string(VERSION_PATCH));

const string APP_NAME = string("Quik Connector (version: ").append(APP_VERSION).append(")");

typedef struct LogConfigDto {
    string level;
    string path;
    string name;
    int fileMaxSize;
    int maxFiles;
} LogConfigDto;

typedef struct RedisConfigDto {
    bool isEnabled;
    string host;
    int port;
    Option<string> password;
} RedisConfigDto;

typedef struct DbConfigDto {
    string path;
    string name;
    string migrationsPath;
} DbConfigDto;

typedef struct QuikCallbackConfigDto {
    bool onAllTradeEnabled;
    bool onQuoteEnabled;
    bool onOrderEnabled;
    bool onTransReplyEnabled;
} QuikCallbackConfigDto;

typedef struct QuikOrderConfigDto {
    bool ignoreCancelled;
    bool saveToDb;
} QuikOrderConfigDto;

typedef struct QuikNewsConfigDto {
    string fileName;
    bool monitorUpdatesEnabled;
} QuikNewsConfigDto;

typedef struct QuikConfigDto {
    QuikCallbackConfigDto callback;
    QuikOrderConfigDto order;
    QuikNewsConfigDto news;
} QuikConfigDto;

typedef struct DebugConfigDto {
    bool printLuaStack;
} DebugConfigDto;

typedef struct ConfigDto {
    ConfigDto(): directorySeparator(DIRECTORY_SEPARATOR) {};
    string directorySeparator;
    string scriptPath;
    LogConfigDto log;
    RedisConfigDto redis;
    DbConfigDto db;
    QuikConfigDto quik;
    DebugConfigDto debug;
} ConfigDto;

#endif //QUIK_CONNECTOR_CONFIG_H
