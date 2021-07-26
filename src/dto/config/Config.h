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

using namespace std;

const string APP_NAME = "Quik Connector";

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

typedef struct ConfigDto {
    ConfigDto(): directorySeparator(DIRECTORY_SEPARATOR) {};
    string directorySeparator;
    string version;
    string scriptPath;
    LogConfigDto log;
    RedisConfigDto redis;
    OrderConfigDto order;
} ConfigDto;

#endif //QUIK_CONNECTOR_CONFIG_H
