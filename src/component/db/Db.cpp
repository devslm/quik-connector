//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include "Db.h"

Db::Db() {
    const ConfigDto config = configService->getConfig();
    string dbPath = config.scriptPath + config.directorySeparator + config.db.path;
    string dbMigrationsPath = dbPath + config.directorySeparator + config.db.migrationsPath;
    string dbFilePath = dbPath + config.directorySeparator + config.db.name;

    LOGGER->info("Open DB with path: {}, name: {} and migrations sub path: {}",
        config.db.path, config.db.name, config.db.migrationsPath);

    FileUtils::createdDirs(dbPath);
    FileUtils::createdDirs(dbMigrationsPath);

    this->db = new SQLite::Database(dbFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    runMigrations(dbMigrationsPath);
}

SQLite::Database* Db::getConnection() {
    return this->db;
}

double Db::toSqlLiteBigInt(uint64_t value) {
    return static_cast<double>(value);
}

/**
 * Simple migrations implementation just for execute SQL queries from files.
 *
 * @param dbMigrationsPath the path with migration files
 */
void Db::runMigrations(string& dbMigrationsPath) {
    LOGGER->info("Apply DB migrations...");

    try {
        set<string> migrations = FileUtils::getFiles(dbMigrationsPath);

        if (migrations.empty()) {
            LOGGER->info("New migrations not found, skipping...");
            return;
        }
        SQLite::Transaction transaction(*db);

        set<string> appliedMigrations = getAppliedMigrations();
        int lastHistoryId = (int)(appliedMigrations.size() + 1);
        int totalAppliedMigrations = 0;

        for (const auto& migrationFile : migrations) {
            if (appliedMigrations.find(migrationFile) != appliedMigrations.end()) {
                continue;
            }
            LOGGER->info("Apply new migration: {}", migrationFile);

            string migrationFilePath = dbMigrationsPath.append("/").append(migrationFile);
            string sqlScript = FileUtils::readFile(migrationFilePath);

            db->exec(sqlScript);

            SQLite::Statement query(*db, "INSERT INTO schema_history (id, script) VALUES (?, ?)");
            query.bind(1, ++lastHistoryId);
            query.bind(2, migrationFile);
            query.exec();

            ++totalAppliedMigrations;
        }
        transaction.commit();

        if (totalAppliedMigrations == 0) {
            LOGGER->info("No one new DB migration found, skipping...");
        } else {
            LOGGER->info("New DB migrations applied successfully");
        }
    } catch (SQLite::Exception& exception) {
        LOGGER->error("Could not apply DB migrations! Reason: {}", exception.what());
    }
}

set<string> Db::getAppliedMigrations() {
    db->exec(
        "CREATE TABLE IF NOT EXISTS schema_history ("
            "id INT NOT NULL PRIMARY KEY, "
            "script VARCHAR(256) NOT NULL, "
            "date_create TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")");

    SQLite::Statement query(*db, "SELECT id, script FROM schema_history");

    set<string> appliedMigrations;

    while (query.executeStep()) {
        appliedMigrations.insert(query.getColumn(1));
    }
    return appliedMigrations;
}
