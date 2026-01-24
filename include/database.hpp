/**
 * @file database.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
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
  // Singleton access
  static DatabaseManager &instance();

  // Initialization (called once in main.cpp)
  void initialize(const QString &path);

  // Returns the DB connection for the CURRENT thread
  QSqlDatabase getDatabase();

  // Executes schema setup (create tables)
  bool migrate();

private:
  DatabaseManager() = default;
  ~DatabaseManager();

  // Prevent copying
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager &) = delete;

  QString m_dbPath;
};
