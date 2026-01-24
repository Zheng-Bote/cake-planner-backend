/**
 * @file token_utils.hpp
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
#include <jwt-cpp/jwt.h>
#include <optional>

namespace rz {
namespace utils {

struct TokenPayload {
  QString userId;
  QString email;
  bool isAdmin;
};

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
