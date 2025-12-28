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

  // --- NEU: Ordner-Check ---
  QFileInfo fileInfo(m_dbPath);
  QDir dir = fileInfo.absoluteDir();

  if (!dir.exists()) {
    qInfo() << "Datenbank-Ordner existiert nicht. Erstelle:"
            << dir.absolutePath();
    if (!dir.mkpath(".")) {
      qCritical()
          << "Kritischer Fehler: Konnte Datenbank-Ordner nicht erstellen:"
          << dir.absolutePath();
      // Wir machen weiter, aber der nächste Schritt wird vermutlich crashen
    }
  }
  // -------------------------

  qInfo() << "Datenbank-Pfad gesetzt auf:" << m_dbPath;
}

QSqlDatabase DatabaseManager::getDatabase() {
  // 1. Eindeutige ID für den aktuellen Thread generieren
  // Qt's QThread::currentThreadId() eignet sich gut als Suffix
  QString connectionName =
      QString("db_conn_%1")
          .arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));

  // 2. Prüfen, ob für diesen Thread schon eine Verbindung existiert
  if (QSqlDatabase::contains(connectionName)) {
    auto db = QSqlDatabase::database(connectionName);
    if (db.isOpen()) {
      return db;
    }
    // Falls sie existiert aber geschlossen ist, versuchen wir sie erneut zu
    // öffnen
    if (!db.open()) {
      qCritical()
          << "Kritischer Fehler: Konnte existierende Verbindung nicht öffnen:"
          << connectionName;
    }
    return db;
  }

  // 3. Neue Verbindung für diesen Thread erstellen
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
  db.setDatabaseName(m_dbPath);

  if (!db.open()) {
    qCritical() << "Fehler beim Öffnen der DB in Thread" << connectionName
                << ":" << db.lastError().text();
  } else {
    // Performance-Tuning für SQLite (wichtig für Concurrent Access!)
    QSqlQuery query(db);
    query.exec("PRAGMA journal_mode = WAL;"); // Write-Ahead Logging für bessere
                                              // Concurrency
    query.exec("PRAGMA synchronous = NORMAL;");
    query.exec("PRAGMA foreign_keys = ON;");
  }

  return db;
}

DatabaseManager::~DatabaseManager() {
  // Optional: Aufräumen, obwohl Qt das beim Beenden der QCoreApplication meist
  // selbst macht
}

// Hier fügen wir das Schema ein, das wir vorhin erstellt haben
bool DatabaseManager::migrate() {
  // Wir holen uns eine temporäre Verbindung im Main-Thread
  auto db = getDatabase();

  // Das SQL-Schema als Raw String Literal (C++11 feature, sehr praktisch hier)
  QString schemaSql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            full_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            totp_secret TEXT,
            is_active INTEGER DEFAULT 0,
            is_admin INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS groups (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS group_members (
            user_id TEXT NOT NULL,
            group_id TEXT NOT NULL,
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
        
        -- Indizes fehlen hier noch der Kürze halber, können aber ergänzt werden
    )";

  // SQLite kann oft nicht mehrere Statements in einem "exec" ausführen,
  // daher splitten wir am Semikolon (einfache Implementierung)
  QStringList statements = schemaSql.split(';', Qt::SkipEmptyParts);

  QSqlQuery query(db);
  bool success = true;

  db.transaction();
  for (const QString &stmt : statements) {
    // 1. Leerzeichen entfernen
    QString trimmedStmt = stmt.trimmed();

    // 2. Leere Anweisungen überspringen
    if (trimmedStmt.isEmpty())
      continue;

    if (trimmedStmt.startsWith("--"))
      continue;

    // 4. Ausführen (jetzt nutzen wir die gesäuberte 'trimmedStmt')
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