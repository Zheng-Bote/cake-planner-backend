/**
 * @file seeder.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
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

void Seeder::ensureAdminExists() {
  if (User::existsAnyAdmin()) {
    qInfo() << "System-Check: Admin-Account existiert bereits.";
    return;
  }

  qWarning()
      << "System-Check: Kein Admin gefunden. Erstelle initialen Admin...";

  QString adminPw = EnvLoader::get("CAKE_ADMIN_PASSWORD", "admin123");

  User admin;
  admin.full_name = "System Administrator";
  admin.email = "admin@cakeplanner.local"; // Standard-Email
  admin.password_hash = PasswordUtils::hashPassword(adminPw);
  admin.is_active = true; // Admin ist sofort aktiv
  admin.is_admin = true;  // Wichtig!

  if (admin.create()) {
    qInfo() << "ERFOLG: Initialer Admin erstellt.";
    qInfo() << "Login Email: admin@cakeplanner.local";
    qInfo() << "Passwort: (siehe CAKE_ADMIN_PASSWORD in .env)";
  } else {
    qCritical() << "FEHLER: Konnte initialen Admin nicht erstellen!";
  }
}