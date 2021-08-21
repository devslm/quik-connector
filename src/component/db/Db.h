//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_DB_H
#define QUIK_CONNECTOR_DB_H

#include <string>
#include <set>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include "../../service/config/ConfigService.h"
#include "../../service/quik/utils/FileUtils.h"
#include "../../service/log/Logger.h"
#include "../../dto/config/Config.h"
#include "../../dto/option/Option.h"

using namespace std;
using namespace SQLite;

class ConfigService;

extern ConfigService *configService;
extern shared_ptr<spdlog::logger> LOGGER;

class Db {
public:
    Db();

    ~Db() = default;

    Database* getConnection();

    void runMigrations(string& dbMigrationsPath);

    static double toSqlLiteBigInt(uint64_t value);

private:
    Database* db;

    set<string> getAppliedMigrations();
};

#endif //QUIK_CONNECTOR_DB_H
