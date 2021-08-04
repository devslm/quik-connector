//
// Created by Sergey on 26.06.2021.
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
    string host;
    int port;
} RedisConfigDto;

typedef struct OrderConfigDto {
    bool ignoreCancelled;
} OrderConfigDto;

typedef struct DbConfigDto {
    string path;
    string name;
    string migrationsPath;
} DbConfigDto;

typedef struct DebugConfigDto {
    bool printLuaStack;
} DebugConfigDto;

typedef struct ConfigDto {
    ConfigDto(): directorySeparator(DIRECTORY_SEPARATOR) {};
    string directorySeparator;
    string scriptPath;
    LogConfigDto log;
    RedisConfigDto redis;
    OrderConfigDto order;
    DbConfigDto db;
    DebugConfigDto debug;
} ConfigDto;

#endif //QUIK_CONNECTOR_CONFIG_H
