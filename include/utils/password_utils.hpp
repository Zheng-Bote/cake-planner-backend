/**
 * @file password_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>
#include <string>

namespace rz {
namespace utils {

class PasswordUtils {
public:
  /**
   * Creates an Argon2id hash.
   * Return format: $argon2id$v=19$m=65536,t=3,p=4$...salt...$hash...
   */
  static QString hashPassword(const QString &plainText);

  /**
   * Verifies a password against an Argon2 hash string.
   */
  static bool verifyPassword(const QString &plainText,
                             const QString &encodedHash);

  /**
   * Generates a random variable-length safe string.
   */
  static QString generateRandomPassword(int length = 12);
};

} // namespace utils
} // namespace rz
