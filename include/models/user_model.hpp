/**
 * @file user_model.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Model with Group Management Extensions
 * @version 0.2.1
 * @date 2026-01-07
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
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
  QString emailLanguage = "en";
  QString totp_secret;
  QString groupId;
  QString groupName;
  QString groupRole;
  bool is_active;
  bool is_admin;
  bool must_change_password = false;

  static bool setMustChangePassword(const QString &userId, bool mustChange);
  bool enable2FA(const QString &secret);

  // --- View Logic (Serialization) ---
  crow::json::wvalue toJson() const;

  // --- Business / DB Logic ---
  static std::optional<User> getById(const QString &id);
  static std::optional<User> getByEmail(const QString &email);
  static std::vector<User> getAll(const QString &filterGroupId = "");
  static std::pair<QString, QString> getGroupAndRole(const QString &userId);

  static bool updateStatus(const QString &userId, bool isActive);
  static bool updatePassword(const QString &userId, const QString &newHash);
  static bool updateSettings(const QString& userId, const QString& lang);

  // --- Group Management ---
  static std::vector<std::pair<QString, QString>> getAllGroups();
  static bool assignToGroup(const QString &userId, const QString &groupId);
  static bool setGroupRole(const QString &userId, const QString &groupId, const QString &role);
  static QString getGroupRole(const QString &userId, const QString &groupId);

  // NEU: Admin-Aktionen für Gruppen
  static QString createGroup(const QString &name);
  static bool deleteGroup(const QString &groupId);

  static bool softDelete(const QString& userId);
  static bool existsAnyAdmin();

  bool create();
};
