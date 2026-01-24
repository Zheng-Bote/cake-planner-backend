/**
 * @file user_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Model Implementation
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
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

/**
 * @brief Retrieves the group ID and role for a specific user.
 *
 * @param userId The ID of the user.
 * @return A pair containing the group ID (first) and the role (second).
 */
std::pair<QString, QString> User::getGroupAndRole(const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare(
      "SELECT group_id, role FROM group_members WHERE user_id = :uid");
  query.bindValue(":uid", userId);

  if (query.exec() && query.next()) {
    return {query.value("group_id").toString(), query.value("role").toString()};
  }
  return {}; // Empty if no group
}

// --- Business / DB Logic ---

/**
 * @brief Converts the User object to a JSON value.
 *
 * @return A crow::json::wvalue containing the user data.
 */
crow::json::wvalue User::toJson() const {
  crow::json::wvalue json;
  json["id"] = id.toStdString();
  json["name"] = full_name.toStdString();
  json["email"] = email.toStdString();

  // FIX 1: Include language in JSON
  json["language"] = language.toStdString();
  json["emailLanguage"] = emailLanguage.toStdString();

  json["isAdmin"] = is_admin;
  json["isActive"] = is_active;
  json["mustChangePassword"] = must_change_password;
  json["has2FA"] = !totp_secret.isEmpty();
  json["groupId"] = groupId.toStdString();
  json["groupRole"] = groupRole.toStdString();
  json["lastLoginAt"] = last_login_at.toStdString();
  return json;
}

/**
 * @brief Retrieves a user by their email address.
 *
 * @param email The email address to search for.
 * @return An optional User object if found, std::nullopt otherwise.
 */
std::optional<User> User::getByEmail(const QString &email) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  // FIX 2: Add email_language in SELECT
  query.prepare("SELECT id, full_name, email, language, email_language, password_hash, is_active, "
                "is_admin, totp_secret, must_change_password, temp_password_hash, temp_password_expiry, last_login_at FROM users WHERE "
                "email = :email");
  query.bindValue(":email", email);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();

    // Read language
    u.language = query.value("language").toString();
    if(u.language.isEmpty()) u.language = "en";
    u.emailLanguage = query.value("email_language").toString();
    if(u.emailLanguage.isEmpty()) u.emailLanguage = "en";

    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();
    u.temp_password_hash = query.value("temp_password_hash").toString();
    u.temp_password_expiry = query.value("temp_password_expiry").toString();
    u.last_login_at = query.value("last_login_at").toString();
    // Reload group info
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

/**
 * @brief Retrieves a user by their unique ID.
 *
 * @param id The ID of the user.
 * @return An optional User object if found, std::nullopt otherwise.
 */
std::optional<User> User::getById(const QString &id) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  // FIX 3: Add email_language in SELECT
  query.prepare(
      "SELECT id, full_name, email, language, email_language, password_hash, is_active, "
      "is_admin, totp_secret, must_change_password, temp_password_hash, temp_password_expiry, last_login_at FROM users WHERE id = :id");
  query.bindValue(":id", id);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();

    // Read language
    u.language = query.value("language").toString();
    if(u.language.isEmpty()) u.language = "en";
    u.emailLanguage = query.value("email_language").toString();
    if(u.emailLanguage.isEmpty()) u.emailLanguage = "en";

    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();
    u.temp_password_hash = query.value("temp_password_hash").toString();
    u.temp_password_expiry = query.value("temp_password_expiry").toString();
    u.last_login_at = query.value("last_login_at").toString();

    // Reload group info
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

/**
 * @brief Retrieves all users, optionally filtered by group.
 *
 * @param filterGroupId Optional group ID to filter users by.
 * @return A vector of User objects.
 */
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

/**
 * @brief Checks if any admin user exists in the database.
 *
 * @return True if at least one admin exists, false otherwise.
 */
bool User::existsAnyAdmin() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT COUNT(*) FROM users WHERE is_admin = 1");
  if (query.exec() && query.next()) {
    return query.value(0).toInt() > 0;
  }
  return false;
}

/**
 * @brief Creates a new user in the database.
 *
 * @return True if successful, false otherwise.
 */
bool User::create() {
  auto db = DatabaseManager::instance().getDatabase();

  if (this->id.isEmpty()) {
    this->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }

  QSqlQuery query(db);
  // FIX 4: Save email_language
  query.prepare("INSERT INTO users (id, full_name, email, password_hash, "
                "is_active, is_admin, email_language, language) "
                "VALUES (:id, :name, :email, :pass, :active, :admin, :emaillang, :lang)");
  query.bindValue(":id", this->id);
  query.bindValue(":name", this->full_name);
  query.bindValue(":email", this->email);
  query.bindValue(":pass", this->password_hash);
  query.bindValue(":active", this->is_active ? 1 : 0);
  query.bindValue(":admin", this->is_admin ? 1 : 0);

  // Set default language on creation
  query.bindValue(":emaillang", this->emailLanguage.isEmpty() ? "en" : this->emailLanguage);
  query.bindValue(":lang", this->language.isEmpty() ? "en" : this->language);

  return query.exec();
}

/**
 * @brief Enables Two-Factor Authentication (2FA) for the user.
 *
 * @param secret The TOTP secret key.
 * @return True if successful, false otherwise.
 */
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

/**
 * @brief Updates the active status of a user.
 *
 * @param userId The ID of the user.
 * @param isActive The new active status.
 * @return True if successful, false otherwise.
 */
bool User::updateStatus(const QString &userId, bool isActive) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE users SET is_active = :active WHERE id = :id");
  query.bindValue(":active", isActive);
  query.bindValue(":id", userId);

  return query.exec();
}

/**
 * @brief Sets the "must change password" flag for a user.
 *
 * @param userId The ID of the user.
 * @param mustChange True if the user must change their password, false otherwise.
 * @return True if successful, false otherwise.
 */
bool User::setMustChangePassword(const QString &userId, bool mustChange) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET must_change_password = :val WHERE id = :id");
  query.bindValue(":val", mustChange ? 1 : 0);
  query.bindValue(":id", userId);
  return query.exec();
}

/**
 * @brief Updates the user's password hash.
 *
 * This also resets the "must change password" flag.
 *
 * @param userId The ID of the user.
 * @param newHash The new password hash.
 * @return True if successful, false otherwise.
 */
bool User::updatePassword(const QString &userId, const QString &newHash) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE users SET password_hash = :hash, must_change_password "
                "= 0 WHERE id = :id");
  query.bindValue(":hash", newHash);
  query.bindValue(":id", userId);

  return query.exec();
}

/**
 * @brief Sets a temporary password for the user.
 *
 * @param hash The hash of the temporary password.
 * @param durationInHours How long the password is valid.
 * @return True if successful.
 */
bool User::setTempPassword(const QString &hash, int durationInHours) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    QDateTime expiry = QDateTime::currentDateTime().addSecs(durationInHours * 3600);

    query.prepare("UPDATE users SET temp_password_hash = :hash, temp_password_expiry = :expiry WHERE id = :id");
    query.bindValue(":hash", hash);
    query.bindValue(":expiry", expiry.toString(Qt::ISODate));
    query.bindValue(":id", this->id);

    return query.exec();
}

/**
 * @brief Clears the temporary password for the user.
 *
 * @return True if successful.
 */
bool User::clearTempPassword() {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE users SET temp_password_hash = NULL, temp_password_expiry = NULL WHERE id = :id");
    query.bindValue(":id", this->id);
    return query.exec();
}

/**
 * @brief Updates the last login timestamp for a user.
 *
 * @param userId The ID of the user.
 * @return True if successful, false otherwise.
 */

bool User::touchLastLogin(const QString &userId) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    // We use Qt::ISODate for a standard format (YYYY-MM-DDTHH:mm:ss)
    query.prepare("UPDATE users SET last_login_at = :time WHERE id = :id");
    query.bindValue(":time", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":id", userId);

    if (!query.exec()) {
        spdlog::error("Failed to update last_login_at for user {}: {}",
                      userId.toStdString(),
                      query.lastError().text().toStdString());
        return false;
    }
    return true;
}

/**
 * @brief Retrieves all existing user groups.
 *
 * @return A vector of pairs, where each pair contains the group ID and group name.
 */
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

/**
 * @brief Assigns a user to a group.
 *
 * Removes the user from any existing group before assigning the new one.
 *
 * @param userId The ID of the user.
 * @param groupId The ID of the group.
 * @return True if successful, false otherwise.
 */
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

/**
 * @brief Sets the role of a user within a group.
 *
 * @param userId The ID of the user.
 * @param groupId The ID of the group.
 * @param role The new role (e.g., "admin", "member").
 * @return True if successful, false otherwise.
 */
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

/**
 * @brief Retrieves the role of a user within a specific group.
 *
 * @param userId The ID of the user.
 * @param groupId The ID of the group.
 * @return The role as a string, or an empty string if not found.
 */
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

/**
 * @brief Soft-deletes a user account.
 *
 * The user data is anonymized, and the account is deactivated.
 *
 * @param userId The ID of the user to delete.
 * @return True if successful, false otherwise.
 */
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

/**
 * @brief Updates the preferred language for email notifications.
 *
 * @param userId The ID of the user.
 * @param lang The language code (e.g., "en", "de").
 * @return True if successful, false otherwise.
 */
bool User::updateEmailLanguage(const QString& userId, const QString& lang) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE users SET email_language = :lang WHERE id = :id");
    query.bindValue(":lang", lang);
    query.bindValue(":id", userId);
    return query.exec();
}

/**
 * @brief Updates the preferred UI language.
 *
 * @param userId The ID of the user.
 * @param lang The language code (e.g., "en", "de").
 * @return True if successful, false otherwise.
 */
bool User::updateLanguage(const QString& userId, const QString& lang) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("UPDATE users SET language = :lang WHERE id = :id");
    query.bindValue(":lang", lang);
    query.bindValue(":id", userId);
    return query.exec();
}

/**
 * @brief Creates a new group in the database.
 *
 * @param name The name of the group.
 * @return The new group ID (UUID) or an empty string on error.
 */
QString User::createGroup(const QString &name) {
    QString trimmedName = name.trimmed();
    if (trimmedName.isEmpty()) {
        spdlog::warn("createGroup aborted: Name is empty.");
        return "";
    }

    // Get database connection (Thread-Safe via DatabaseManager)
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QString newGroupId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(db);
    query.prepare("INSERT INTO groups (id, name) VALUES (:id, :name)");
    query.bindValue(":id", newGroupId);
    query.bindValue(":name", trimmedName);

    if (!query.exec()) {
        spdlog::error("Error creating group '{}': {}",
                      trimmedName.toStdString(),
                      query.lastError().text().toStdString());
        return "";
    }

    spdlog::info("Group successfully created: {} (ID: {})",
                 trimmedName.toStdString(),
                 newGroupId.toStdString());

    return newGroupId;
}

/**
 * @brief Deletes a group from the database.
 *
 * Before deletion, it checks if any users are still assigned to this group.
 *
 * @param groupId The ID of the group to delete.
 * @return True if deleted, false if the group is not empty or on DB error.
 */
bool User::deleteGroup(const QString &groupId) {
    if (groupId.isEmpty()) return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();

    // 1. Integrity check: Are there still users in the group?
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE group_id = :gid");
    checkQuery.bindValue(":gid", groupId);

    if (checkQuery.exec() && checkQuery.next()) {
        int userCount = checkQuery.value(0).toInt();
        if (userCount > 0) {
            spdlog::warn("Deleting group {} denied: {} users are still assigned.",
                         groupId.toStdString(), userCount);
            return false; // Prevents orphaned user entries
        }
    }

    // 2. Actual Deletion
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM groups WHERE id = :id");
    deleteQuery.bindValue(":id", groupId);

    if (!deleteQuery.exec()) {
        spdlog::error("Error deleting group {}: {}",
                      groupId.toStdString(),
                      deleteQuery.lastError().text().toStdString());
        return false;
    }

    if (deleteQuery.numRowsAffected() == 0) {
        spdlog::warn("Deletion failed: Group {} does not exist.", groupId.toStdString());
        return false;
    }

    spdlog::info("Group {} successfully deleted.", groupId.toStdString());
    return true;
}


std::vector<User::GroupMembership> User::getGroupsForUser(const QString &userId) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    std::vector<GroupMembership> result;

    // We get ID and Name from the groups table and the role from the join table
    QString sql = R"(
        SELECT g.id, g.name, gm.role
        FROM groups g
        INNER JOIN group_members gm ON g.id = gm.group_id
        WHERE gm.user_id = :uid
        ORDER BY g.name ASC
    )";

    query.prepare(sql);
    query.bindValue(":uid", userId);

    if (query.exec()) {
        while (query.next()) {
            GroupMembership gm;
            gm.groupId = query.value("id").toString();
            gm.groupName = query.value("name").toString();
            gm.role = query.value("role").toString();
            result.push_back(gm);
        }
    } else {
        qWarning() << "User::getGroupsForUser error:" << query.lastError().text();
    }

    return result;
}
