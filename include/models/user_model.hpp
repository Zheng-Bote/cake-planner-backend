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
  QString totp_secret;
  QString groupId;
  QString groupName;
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
  static std::vector<User> getAll();
  static bool updateStatus(const QString &userId, bool isActive);
  static bool updatePassword(const QString &userId, const QString &newHash);

  static std::vector<std::pair<QString, QString>>
  getAllGroups(); // Gibt ID + Name zurück
  static bool assignToGroup(const QString &userId, const QString &groupId);

  static bool existsAnyAdmin();

  bool create();
};