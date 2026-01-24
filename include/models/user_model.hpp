/**
 * @file user_model.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Model with Group Management Extensions
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow/json.h"
#include <QDateTime>
#include <QString>
#include <optional>
#include <vector>

struct User {
  QString id;
  QString full_name;
  QString email;
  QString password_hash;
  QString language = "en";
  QString emailLanguage = "en";
  QString totp_secret;
  QString groupId;
  QString groupName;
  QString groupRole;
  bool is_active;
  bool is_admin;
  bool must_change_password = false;
  QString last_login_at;

  // Temp Password
  QString temp_password_hash;
  QString temp_password_expiry; // ISO Date string

  struct GroupMembership {
      QString groupId;
      QString groupName;
      QString role;
  };

  static bool setMustChangePassword(const QString &userId, bool mustChange);
  bool setTempPassword(const QString &hash, int durationInHours = 24);
  bool clearTempPassword();

  bool enable2FA(const QString &secret);

  // --- View Logic (Serialization) ---
  crow::json::wvalue toJson() const;

  // --- Business / DB Logic ---
  static std::optional<User> getById(const QString &id);
  static std::optional<User> getByEmail(const QString &email);
  static std::vector<User> getAll(const QString &filterGroupId = "");
  static std::pair<QString, QString> getGroupAndRole(const QString &userId);
  static bool touchLastLogin(const QString &userId);

  static bool updateStatus(const QString &userId, bool isActive);
  static bool updatePassword(const QString &userId, const QString &newHash);
  static bool updateLanguage(const QString& userId, const QString& lang);
  static bool updateEmailLanguage(const QString& userId, const QString& lang);

  // --- Group Management ---
  static std::vector<std::pair<QString, QString>> getAllGroups();
  static bool assignToGroup(const QString &userId, const QString &groupId);
  static bool setGroupRole(const QString &userId, const QString &groupId, const QString &role);
  static QString getGroupRole(const QString &userId, const QString &groupId);
static std::vector<GroupMembership> getGroupsForUser(const QString &userId);

  // NEW: Admin actions for groups
  static QString createGroup(const QString &name);
  static bool deleteGroup(const QString &groupId);

  static bool softDelete(const QString& userId);
  static bool existsAnyAdmin();

  bool create();
};
