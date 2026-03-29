/**
 * SPDX-FileComment: No description provided
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file token_utils.hpp
 * @brief No description provided
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include <QString>
#include <jwt-cpp/jwt.h>
#include <optional>

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief utils namespace.
 */
namespace utils {

/**
 * @brief TokenPayload struct.
 */
struct TokenPayload {
  QString userId;
  QString email;
  bool isAdmin;
};

/**
 * @brief TokenUtils class.
 */
class TokenUtils {
public:
  // Generates a token, valid for 24h
  static QString generateToken(const QString &userId, const QString &email,
                               bool isAdmin);

  // Verifies the token and returns payload (or nullopt on error)
  static std::optional<TokenPayload> verifyToken(const std::string &rawToken);
};

} // namespace utils
} // namespace rz
