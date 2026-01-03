/**
 * @file seeder.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Seeder Utilities
 * @version 0.1.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils/seeder.hpp"
#include "models/user_model.hpp"
#include "utils/env_loader.hpp"
#include "utils/password_utils.hpp"
#include <QDebug>

// Namespace rz::utils
namespace rz {
namespace utils {

void Seeder::ensureAdminExists() {
  if (User::existsAnyAdmin()) {
    qInfo() << "System-Check: Admin-Account existiert bereits.";
    return;
  }

  qWarning()
      << "System-Check: Kein Admin gefunden. Erstelle initialen Admin...";

  // Zugriff auf EnvLoader im gleichen Namespace
  QString adminPw = EnvLoader::get("CAKE_ADMIN_PASSWORD", "admin123");

  User admin;
  admin.full_name = "System Administrator";
  admin.email = "admin@cakeplanner.local";
  // Zugriff auf PasswordUtils im gleichen Namespace
  admin.password_hash = PasswordUtils::hashPassword(adminPw);
  admin.is_active = true;
  admin.is_admin = true;

  if (admin.create()) {
    qInfo() << "ERFOLG: Initialer Admin erstellt.";
    qInfo() << "Login Email: admin@cakeplanner.local";
  } else {
    qCritical() << "FEHLER: Konnte initialen Admin nicht erstellen!";
  }
}

} // namespace utils
} // namespace rz
