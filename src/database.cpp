/**
 * @file database.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.2.0
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
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

DatabaseManager &DatabaseManager::instance() {
  static DatabaseManager instance;
  return instance;
}

void DatabaseManager::initialize(const QString &path) {
  m_dbPath = path;

  // Ordner-Check
  QFileInfo fileInfo(m_dbPath);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists()) {
    qInfo() << "Datenbank-Ordner existiert nicht. Erstelle:"
            << dir.absolutePath();
    if (!dir.mkpath(".")) {
      qCritical()
          << "Kritischer Fehler: Konnte Datenbank-Ordner nicht erstellen:"
          << dir.absolutePath();
    }
  }

  qInfo() << "Datenbank-Pfad gesetzt auf:" << m_dbPath;
}

QSqlDatabase DatabaseManager::getDatabase() {
  QString connectionName =
      QString("db_conn_%1")
          .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

  if (QSqlDatabase::contains(connectionName)) {
    auto db = QSqlDatabase::database(connectionName);
    if (db.isOpen()) return db;
    if (!db.open()) {
      qCritical() << "Kritischer Fehler: Konnte existierende Verbindung nicht öffnen:" << connectionName;
    }
    return db;
  }

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
  db.setDatabaseName(m_dbPath);

  if (!db.open()) {
    qCritical() << "Fehler beim Öffnen der DB in Thread" << connectionName
                << ":" << db.lastError().text();
  } else {
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode = WAL;");
    query.exec("PRAGMA synchronous = NORMAL;");
    query.exec("PRAGMA foreign_keys = ON;");
  }

  return db;
}

DatabaseManager::~DatabaseManager() {}

bool DatabaseManager::migrate() {
  auto db = getDatabase();

  QString schemaSql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            full_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            totp_secret TEXT,
            is_active INTEGER DEFAULT 0,
            is_admin INTEGER DEFAULT 0,
            must_change_password INTEGER DEFAULT 0,
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

        -- INDIZES für Performance
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
      qCritical() << "Migration Fehler bei Statement:" << trimmedStmt
                  << "\nGrund:" << query.lastError().text();
      success = false;
      break;
    }
  }

  if (success) {
    db.commit();
    qInfo() << "Datenbank-Migration erfolgreich abgeschlossen.";
  } else {
    db.rollback();
  }

  return success;
}
