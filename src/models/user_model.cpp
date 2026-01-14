/**
 * @file user_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Model Implementation
 * @version 0.4.1
 * @date 2026-01-07
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "models/user_model.hpp"
#include "database.hpp"
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

#include "spdlog/spdlog.h"

// --- Helpers ---

std::pair<QString, QString> User::getGroupAndRole(const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare(
      "SELECT group_id, role FROM group_members WHERE user_id = :uid");
  query.bindValue(":uid", userId);

  if (query.exec() && query.next()) {
    return {query.value("group_id").toString(), query.value("role").toString()};
  }
  return {}; // Leer, falls keine Gruppe
}

// --- Business / DB Logic ---

crow::json::wvalue User::toJson() const {
  crow::json::wvalue json;
  json["id"] = id.toStdString();
  json["name"] = full_name.toStdString();
  json["email"] = email.toStdString();

  // FIX 1: Sprache mit ins JSON aufnehmen
  json["language"] = language.toStdString();
  json["emailLanguage"] = emailLanguage.toStdString();

  json["isAdmin"] = is_admin;
  json["isActive"] = is_active;
  json["mustChangePassword"] = must_change_password;
  json["has2FA"] = !totp_secret.isEmpty();
  json["groupId"] = groupId.toStdString();
  json["groupRole"] = groupRole.toStdString();
  return json;
}

std::optional<User> User::getByEmail(const QString &email) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  // FIX 2: email_language im SELECT hinzufügen
  query.prepare("SELECT id, full_name, email, language, email_language, password_hash, is_active, "
                "is_admin, totp_secret, must_change_password FROM users WHERE "
                "email = :email");
  query.bindValue(":email", email);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();

    // Sprache auslesen
    u.language = query.value("language").toString();
    if(u.language.isEmpty()) u.language = "en";
    u.emailLanguage = query.value("email_language").toString();
    if(u.emailLanguage.isEmpty()) u.emailLanguage = "en";

    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();

    // Gruppen-Infos nachladen
    auto groupInfo = getGroupAndRole(u.id);
    u.groupId = groupInfo.first;
    u.groupRole = groupInfo.second;

    if (u.groupRole.isEmpty() && !u.groupId.isEmpty()) {
      u.groupRole = "member";
    }

    return u;
  }
  return std::nullopt;
}

std::optional<User> User::getById(const QString &id) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  // FIX 3: email_language im SELECT hinzufügen
  query.prepare(
      "SELECT id, full_name, email, language, email_language, password_hash, is_active, "
      "is_admin, totp_secret, must_change_password FROM users WHERE id = :id");
  query.bindValue(":id", id);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();

    // Sprache auslesen
    u.language = query.value("language").toString();
    if(u.language.isEmpty()) u.language = "en";
    u.emailLanguage = query.value("email_language").toString();
    if(u.emailLanguage.isEmpty()) u.emailLanguage = "en";

    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();

    // Gruppen-Infos nachladen
    auto groupInfo = getGroupAndRole(u.id);
    u.groupId = groupInfo.first;
    u.groupRole = groupInfo.second;

    if (u.groupRole.isEmpty() && !u.groupId.isEmpty()) {
      u.groupRole = "member";
    }

    return u;
  }
  return std::nullopt;
}

std::vector<User> User::getAll(const QString &filterGroupId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<User> users;

  QString sql = R"(
        SELECT u.id, u.full_name, u.email, u.language, u.email_language, u.is_active, u.is_admin, u.must_change_password,
               gm.group_id, gm.role
        FROM users u
        LEFT JOIN group_members gm ON u.id = gm.user_id
    )";

  if (!filterGroupId.isEmpty()) {
    sql += " WHERE gm.group_id = :gid";
  }

  query.prepare(sql);

  if (!filterGroupId.isEmpty()) {
    query.bindValue(":gid", filterGroupId);
  }

  if (query.exec()) {
    while (query.next()) {
      User u;
      u.id = query.value("id").toString();
      u.full_name = query.value("full_name").toString();
      u.email = query.value("email").toString();
      u.is_active = query.value("is_active").toBool();
      u.is_admin = query.value("is_admin").toBool();
      u.must_change_password = query.value("must_change_password").toBool();

      u.language = query.value("language").toString();
      if (u.language.isEmpty()) u.language = "en";
      u.emailLanguage = query.value("email_language").toString();
      if (u.emailLanguage.isEmpty()) u.emailLanguage = "en";

      u.groupId = query.value("group_id").toString();
      u.groupRole = query.value("role").toString();
      if (u.groupRole.isEmpty())
        u.groupRole = "member";

      users.push_back(u);
    }
  } else {
    qWarning() << "User::getAll error:" << query.lastError().text();
  }
  return users;
}

bool User::existsAnyAdmin() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT COUNT(*) FROM users WHERE is_admin = 1");
  if (query.exec() && query.next()) {
    return query.value(0).toInt() > 0;
  }
  return false;
}

bool User::create() {
  auto db = DatabaseManager::instance().getDatabase();

  if (this->id.isEmpty()) {
    this->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }

  QSqlQuery query(db);
  // FIX 4: email_language speichern
  query.prepare("INSERT INTO users (id, full_name, email, password_hash, "
                "is_active, is_admin, email_language, language) "
                "VALUES (:id, :name, :email, :pass, :active, :admin, :emaillang, :lang)");
  query.bindValue(":id", this->id);
  query.bindValue(":name", this->full_name);
  query.bindValue(":email", this->email);
  query.bindValue(":pass", this->password_hash);
  query.bindValue(":active", this->is_active ? 1 : 0);
  query.bindValue(":admin", this->is_admin ? 1 : 0);

  // Default Sprache beim Erstellen setzen
  query.bindValue(":emaillang", this->emailLanguage.isEmpty() ? "en" : this->emailLanguage);
  query.bindValue(":lang", this->language.isEmpty() ? "en" : this->language);

  return query.exec();
}

bool User::enable2FA(const QString &secret) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET totp_secret = :secret WHERE id = :id");
  query.bindValue(":secret", secret);
  query.bindValue(":id", this->id);

  if (query.exec()) {
    this->totp_secret = secret;
    return true;
  }
  return false;
}

bool User::updateStatus(const QString &userId, bool isActive) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE users SET is_active = :active WHERE id = :id");
  query.bindValue(":active", isActive);
  query.bindValue(":id", userId);

  return query.exec();
}

bool User::setMustChangePassword(const QString &userId, bool mustChange) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET must_change_password = :val WHERE id = :id");
  query.bindValue(":val", mustChange ? 1 : 0);
  query.bindValue(":id", userId);
  return query.exec();
}

bool User::updatePassword(const QString &userId, const QString &newHash) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE users SET password_hash = :hash, must_change_password "
                "= 0 WHERE id = :id");
  query.bindValue(":hash", newHash);
  query.bindValue(":id", userId);

  return query.exec();
}

std::vector<std::pair<QString, QString>> User::getAllGroups() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<std::pair<QString, QString>> groups;

  if (query.exec("SELECT id, name FROM groups")) {
    while (query.next()) {
      groups.push_back(
          {query.value("id").toString(), query.value("name").toString()});
    }
  }
  return groups;
}

bool User::assignToGroup(const QString &userId, const QString &groupId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("DELETE FROM group_members WHERE user_id = :uid");
  query.bindValue(":uid", userId);
  query.exec();

  query.prepare(
      "INSERT INTO group_members (group_id, user_id) VALUES (:gid, :uid)");
  query.bindValue(":gid", groupId);
  query.bindValue(":uid", userId);

  return query.exec();
}

bool User::setGroupRole(const QString &userId, const QString &groupId,
                        const QString &role) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE group_members SET role = :role WHERE user_id = :uid "
                "AND group_id = :gid");
  query.bindValue(":role", role);
  query.bindValue(":uid", userId);
  query.bindValue(":gid", groupId);

  if (query.exec()) {
    return query.numRowsAffected() > 0;
  }
  return false;
}

QString User::getGroupRole(const QString &userId, const QString &groupId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT role FROM group_members WHERE user_id = :uid AND "
                "group_id = :gid");
  query.bindValue(":uid", userId);
  query.bindValue(":gid", groupId);

  if (query.exec() && query.next()) {
    return query.value("role").toString();
  }
  return "";
}

bool User::softDelete(const QString& userId) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE users
        SET is_active = 0,
            full_name = 'Deleted User',
            email = 'deleted_' || id || '@cakeplanner.local',
            password_hash = '',
            totp_secret = NULL
        WHERE id = :id
    )");
    query.bindValue(":id", userId);
    return query.exec();
}

bool User::updateEmailLanguage(const QString& userId, const QString& lang) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE users SET email_language = :lang WHERE id = :id");
    query.bindValue(":lang", lang);
    query.bindValue(":id", userId);
    return query.exec();
}

bool User::updateLanguage(const QString& userId, const QString& lang) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE users SET language = :lang WHERE id = :id");
    query.bindValue(":lang", lang);
    query.bindValue(":id", userId);
    return query.exec();
}

/**
 * @brief Erstellt eine neue Gruppe in der Datenbank.
 * @param name Der Name der Gruppe.
 * @return Die neue Gruppen-ID (UUID) oder ein leerer String bei Fehler.
 */
QString User::createGroup(const QString &name) {
    QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) {
        spdlog::warn("createGroup abgebrochen: Name ist leer.");
        return "";
    }

    // Datenbankverbindung holen (Thread-Safe über DatabaseManager)
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QString newGroupId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(db);
    query.prepare("INSERT INTO groups (id, name) VALUES (:id, :name)");
    query.bindValue(":id", newGroupId);
    query.bindValue(":name", trimmedName);

    if (!query.exec()) {
        spdlog::error("Fehler beim Erstellen der Gruppe '{}': {}",
                      trimmedName.toStdString(),
                      query.lastError().text().toStdString());
        return "";
    }

    spdlog::info("Gruppe erfolgreich erstellt: {} (ID: {})",
                 trimmedName.toStdString(),
                 newGroupId.toStdString());

    return newGroupId;
}

/**
 * @brief Löscht eine Gruppe aus der Datenbank.
 * Prüft vorher, ob noch Benutzer dieser Gruppe zugeordnet sind.
 * @param groupId Die ID der zu löschenden Gruppe.
 * @return true wenn gelöscht, false wenn Gruppe nicht leer oder DB-Fehler.
 */
bool User::deleteGroup(const QString &groupId) {
    if (groupId.isEmpty()) return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();

    // 1. Integritätsprüfung: Sind noch User in der Gruppe?
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE group_id = :gid");
    checkQuery.bindValue(":gid", groupId);

    if (checkQuery.exec() && checkQuery.next()) {
        int userCount = checkQuery.value(0).toInt();
        if (userCount > 0) {
            spdlog::warn("Löschen der Gruppe {} verweigert: {} Benutzer sind noch zugeordnet.",
                         groupId.toStdString(), userCount);
            return false; // Verhindert verwaiste User-Einträge
        }
    }

    // 2. Eigentliches Löschen
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM groups WHERE id = :id");
    deleteQuery.bindValue(":id", groupId);

    if (!deleteQuery.exec()) {
        spdlog::error("Fehler beim Löschen der Gruppe {}: {}",
                      groupId.toStdString(),
                      deleteQuery.lastError().text().toStdString());
        return false;
    }

    if (deleteQuery.numRowsAffected() == 0) {
        spdlog::warn("Löschen fehlgeschlagen: Gruppe {} existiert nicht.", groupId.toStdString());
        return false;
    }

    spdlog::info("Gruppe {} erfolgreich gelöscht.", groupId.toStdString());
    return true;
}
