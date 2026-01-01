/**
 * @file password_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>
#include <string>

class PasswordUtils {
public:
  /**
   * Erstellt einen Argon2id Hash.
   * Rückgabeformat: $argon2id$v=19$m=65536,t=3,p=4$...salt...$hash...
   */
  static QString hashPassword(const QString &plainText);

  /**
   * Verifiziert ein Passwort gegen einen Argon2 Hash String.
   */
  static bool verifyPassword(const QString &plainText,
                             const QString &encodedHash);
};
