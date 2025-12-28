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
