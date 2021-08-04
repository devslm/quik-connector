//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
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

    this->db = new Database(dbFilePath, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    runMigrations(dbMigrationsPath);
}

Database* Db::getConnection() {
    return this->db;
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
        Transaction transaction(*db);

        db->exec("CREATE TABLE IF NOT EXISTS schema_history (id INT NOT NULL PRIMARY KEY, script VARCHAR(256) NOT NULL, date_create TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

        Statement q(*db, "SELECT id, script FROM schema_history");

        int lastHistoryId;
        set<string> appliedMigrations;

        while (q.executeStep()) {
            lastHistoryId = q.getColumn(0);

            appliedMigrations.insert(q.getColumn(1));
        }
        int totalAppliedMigrations = 0;

        for (const auto& migrationFile : migrations) {
            if (appliedMigrations.find(migrationFile) != appliedMigrations.end()) {
                continue;
            }
            LOGGER->info("Apply new migration: {}", migrationFile);

            string migrationFilePath = dbMigrationsPath.append("/").append(migrationFile);
            string sqlScript = FileUtils::readFile(dbMigrationsPath.append("/").append(migrationFile));

            db->exec(sqlScript);

            Statement query(*db, "INSERT INTO schema_history (id, script) VALUES (?, ?)");
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
    } catch (Exception& exception) {
        LOGGER->error("Could not apply DB migrations! Reason: {}", exception.what());
    }
}
