/**
 * @file database.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <memory>
#include <mutex>

class DatabaseManager {
public:
  // Singleton Zugriff
  static DatabaseManager &instance();

  // Initialisierung (wird einmalig in main.cpp aufgerufen)
  void initialize(const QString &path);

  // Gibt die DB-Verbindung für den AKTUELLEN Thread zurück
  QSqlDatabase getDatabase();

  // Führt das Schema-Setup durch (Tabellen erstellen)
  bool migrate();

private:
  DatabaseManager() = default;
  ~DatabaseManager();

  // Verhindern von Kopien
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  QString m_dbPath;
};
