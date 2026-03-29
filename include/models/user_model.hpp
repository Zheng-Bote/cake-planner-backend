/**
 * SPDX-FileComment: User Model with Group Management Extensions
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file user_model.hpp
 * @brief User Model with Group Management Extensions
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include "crow/json.h"
#include <QDateTime>
#include <QString>
#include <optional>
#include <vector>

/**
 * @brief User struct.
 */
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

  /**
   * @brief GroupMembership struct.
   */
  struct GroupMembership {
      QString groupId;
      QString groupName;
      QString role;
  };

  /**
   * @brief Function implementation.
   */
  static bool setMustChangePassword(const QString &userId, bool mustChange);
  /**
   * @brief Function implementation.
   */
  bool setTempPassword(const QString &hash, int durationInHours = 24);
  /**
   * @brief Function implementation.
   */
  bool clearTempPassword();

  /**
   * @brief Function implementation.
   */
  bool enable2FA(const QString &secret);

  // --- View Logic (Serialization) ---
  crow::json::wvalue toJson() const;

  // --- Business / DB Logic ---
  static std::optional<User> getById(const QString &id);
  /**
   * @brief Function implementation.
   */
  static std::optional<User> getByEmail(const QString &email);
  /**
   * @brief Function implementation.
   */
  static std::vector<User> getAll(const QString &filterGroupId = "");
  /**
   * @brief Function implementation.
   */
  static std::pair<QString, QString> getGroupAndRole(const QString &userId);
  /**
   * @brief Function implementation.
   */
  static bool touchLastLogin(const QString &userId);

  /**
   * @brief Function implementation.
   */
  static bool updateStatus(const QString &userId, bool isActive);
  /**
   * @brief Function implementation.
   */
  static bool updatePassword(const QString &userId, const QString &newHash);
  /**
   * @brief Function implementation.
   */
  static bool updateLanguage(const QString& userId, const QString& lang);
  /**
   * @brief Function implementation.
   */
  static bool updateEmailLanguage(const QString& userId, const QString& lang);

  // --- Group Management ---
  static std::vector<std::pair<QString, QString>> getAllGroups();
  /**
   * @brief Function implementation.
   */
  static bool assignToGroup(const QString &userId, const QString &groupId);
  /**
   * @brief Function implementation.
   */
  static bool setGroupRole(const QString &userId, const QString &groupId, const QString &role);
  /**
   * @brief Function implementation.
   */
  static QString getGroupRole(const QString &userId, const QString &groupId);
/**
 * @brief Function implementation.
 */
static std::vector<GroupMembership> getGroupsForUser(const QString &userId);

  // NEW: Admin actions for groups
  static QString createGroup(const QString &name);
  /**
   * @brief Function implementation.
   */
  static bool deleteGroup(const QString &groupId);

  /**
   * @brief Function implementation.
   */
  static bool softDelete(const QString& userId);
  /**
   * @brief Function implementation.
   */
  static bool existsAnyAdmin();

  /**
   * @brief Function implementation.
   */
  bool create();
};
