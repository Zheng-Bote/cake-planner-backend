/**
 * @file seeder.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Seeder Utilities
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
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

/**
 * @brief Ensures that at least one admin account exists in the system.
 *
 * Checks if any admin user exists. If not, it creates a default admin account
 * with credentials from the environment variables (or defaults).
 */
void Seeder::ensureAdminExists() {
  if (User::existsAnyAdmin()) {
    qInfo() << "System Check: Admin account already exists.";
    return;
  }

  qWarning()
      << "System Check: No admin found. Creating initial admin...";

  // Access EnvLoader in the same namespace
  QString adminPw = EnvLoader::get("CAKE_ADMIN_PASSWORD", "admin123");

  User admin;
  admin.full_name = "System Administrator";
  admin.email = "admin@cakeplanner.local";
  // Access PasswordUtils in the same namespace
  admin.password_hash = PasswordUtils::hashPassword(adminPw);
  admin.is_active = true;
  admin.is_admin = true;

  if (admin.create()) {
    qInfo() << "SUCCESS: Initial admin created.";
    qInfo() << "Login Email: admin@cakeplanner.local";
  } else {
    qCritical() << "ERROR: Could not create initial admin!";
  }
}

} // namespace utils
} // namespace rz
