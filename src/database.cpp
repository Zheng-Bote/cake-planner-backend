/**
 * @file database.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "database.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QThread>
#include <QUuid>

#include <iostream>
#include <sstream>
#include <thread>

#include "spdlog/spdlog.h"

/**
 * @brief Gets the singleton instance of the DatabaseManager.
 *
 * @return Reference to the singleton instance.
 */
DatabaseManager &DatabaseManager::instance() {
  static DatabaseManager instance;
  return instance;
}

/**
 * @brief Initializes the database manager.
 *
 * Sets the database path and ensures the directory exists.
 *
 * @param path The file path to the SQLite database.
 */
void DatabaseManager::initialize(const QString &path) {
  m_dbPath = path;

  // Directory check
  QFileInfo fileInfo(m_dbPath);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists()) {
    qDebug() << "Database directory does not exist. Creating:"
            << dir.absolutePath();
    if (!dir.mkpath(".")) {
      qCritical()
          << "Critical Error: Could not create database directory:"
          << dir.absolutePath();
    }
  }

  qDebug() << "Database path set to:" << m_dbPath;
}

/**
 * @brief Gets a thread-safe database connection.
 *
 * Creates a unique connection name for the current thread to ensure
 * thread safety when accessing the SQLite database.
 *
 * @return A QSqlDatabase connection object.
 */
QSqlDatabase DatabaseManager::getDatabase() {
  QString connectionName =
      QString("db_conn_%1")
          .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

  if (QSqlDatabase::contains(connectionName)) {
    auto db = QSqlDatabase::database(connectionName);
    if (db.isOpen()) return db;
    if (!db.open()) {
      qCritical() << "Critical Error: Could not open existing connection:" << connectionName;
    }
    return db;
  }

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
  db.setDatabaseName(m_dbPath);

  if (!db.open()) {
    qCritical() << "Error opening DB in thread" << connectionName
                << ":" << db.lastError().text();
  } else {
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode = WAL;");
    query.exec("PRAGMA synchronous = NORMAL;");
    query.exec("PRAGMA foreign_keys = ON;");
  }

  return db;
}

/**
 * @brief Destructor.
 */
DatabaseManager::~DatabaseManager() {}

/**
 * @brief Migrates the database schema.
 *
 * Creates necessary tables (users, groups, events, etc.) if they do not exist.
 * Executes the schema SQL in a transaction.
 *
 * @return True if migration succeeded, false otherwise.
 */
bool DatabaseManager::migrate() {
  auto db = getDatabase();

  QString schemaSql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            full_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            language TEXT DEFAULT 'en',
            email_language TEXT DEFAULT 'en',
            totp_secret TEXT,
            is_active INTEGER DEFAULT 0,
            is_admin INTEGER DEFAULT 0,
            must_change_password INTEGER DEFAULT 0,
            last_login_at TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS groups (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS group_members (
            user_id TEXT NOT NULL,
            group_id TEXT NOT NULL,
            role TEXT DEFAULT 'member',
            joined_at TEXT DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (user_id, group_id),
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS events (
            id TEXT PRIMARY KEY,
            group_id TEXT NOT NULL,
            baker_id TEXT NOT NULL,
            event_date TEXT NOT NULL,
            description TEXT,
            photo_path TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE,
            FOREIGN KEY (baker_id) REFERENCES users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS ratings (
            id TEXT PRIMARY KEY,
            event_id TEXT NOT NULL,
            rater_id TEXT NOT NULL,
            rating_value INTEGER NOT NULL CHECK(rating_value >= 1 AND rating_value <= 5),
            comment TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(event_id, rater_id),
            FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE CASCADE,
            FOREIGN KEY (rater_id) REFERENCES users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS event_photos (
            event_id TEXT NOT NULL,
            user_id TEXT NOT NULL,
            photo_path TEXT NOT NULL,
            uploaded_at TEXT DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (event_id, user_id),
            FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE CASCADE,
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
        );

        -- INDICES for Performance
        CREATE INDEX IF NOT EXISTS idx_ratings_event_id ON ratings(event_id);
        CREATE INDEX IF NOT EXISTS idx_event_photos_event_id ON event_photos(event_id);
        CREATE INDEX IF NOT EXISTS idx_events_group_date ON events(group_id, event_date);
    )";

  QStringList statements = schemaSql.split(';', Qt::SkipEmptyParts);
  QSqlQuery query(db);
  bool success = true;

  db.transaction();
  for (const QString &stmt : statements) {
    QString trimmedStmt = stmt.trimmed();
    if (trimmedStmt.isEmpty() || trimmedStmt.startsWith("--")) continue;

    if (!query.exec(trimmedStmt)) {
      qCritical() << "Migration error at statement:" << trimmedStmt
                  << "\nReason:" << query.lastError().text();
    spdlog::error("Failed to create groups table: {}", query.lastError().text().toStdString());
      success = false;
      break;
    }
  }

  if (success) {
    // --- MIGRATION FOR v0.5.0 (Temp Passwords) ---
    // Check if columns exist, if not add them.
    // SQLite does not support IF NOT EXISTS in ALTER TABLE ADD COLUMN directly in all versions,
    // but we can just try to add them and ignore the specific "duplicate column" error,
    // OR inspect the table info. For simplicity/robustness here, we'll try to add and catch error.

    auto checkAndAddColumn = [&](const QString& table, const QString& column, const QString& type) {
        QSqlQuery checkQ(db);
        // Simple check: try to select the column. If it fails, it likely doesn't exist.
        // A cleaner way is PRAGMA table_info, but this is quick.
        checkQ.prepare(QString("SELECT %1 FROM %2 LIMIT 1").arg(column, table));
        if (!checkQ.exec()) {
            // Likely doesn't exist, try to add
             QSqlQuery alterQ(db);
             QString sql = QString("ALTER TABLE %1 ADD COLUMN %2 %3").arg(table, column, type);
             if (!alterQ.exec(sql)) {
                 spdlog::warn("Migration: Failed to add column {}.{} (might already exist or other error): {}",
                    table.toStdString(), column.toStdString(), alterQ.lastError().text().toStdString());
             } else {
                 spdlog::info("Migration: Added column {}.{}", table.toStdString(), column.toStdString());
             }
        }
    };

    checkAndAddColumn("users", "temp_password_hash", "TEXT");
    checkAndAddColumn("users", "temp_password_expiry", "TEXT");

    db.commit();
    qDebug() << "Database migration successfully completed.";
  } else {
    db.rollback();
  }

  return success;
}
