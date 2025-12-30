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
  bool is_active;
  bool is_admin;
  bool enable2FA(const QString &secret);
  // --- View Logic (Serialization) ---
  crow::json::wvalue toJson() const;

  // --- Business / DB Logic ---
  static std::optional<User> getById(const QString &id);
  static std::optional<User> getByEmail(const QString &email);
  static std::vector<User> getAll();
  static bool updateStatus(const QString &userId, bool isActive);

  static bool existsAnyAdmin();

  bool create();
};