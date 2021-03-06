//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DB_H
#define QUIK_CONNECTOR_DB_H

#include <string>
#include <set>
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include "../../service/config/ConfigService.h"
#include "../../service/quik/utils/FileUtils.h"
#include "../../service/log/Logger.h"
#include "../../dto/config/Config.h"
#include "../../dto/option/Option.h"

using namespace std;

class ConfigService;

extern ConfigService *configService;
extern shared_ptr<spdlog::logger> logger;

class Db {
public:
    Db();

    ~Db() = default;

    SQLite::Database* getConnection();

    void runMigrations(string& dbMigrationsPath);

    static long long toSqlLiteBigInt(uint64_t value);

private:
    SQLite::Database* db;

    set<string> getAppliedMigrations();
};

#endif //QUIK_CONNECTOR_DB_H
