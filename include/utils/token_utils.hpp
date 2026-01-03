/**
 * @file token_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.2.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
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
  // Generiert ein Token, gültig für 24h
  static QString generateToken(const QString &userId, const QString &email,
                               bool isAdmin);

  // Verifiziert das Token und gibt Payload zurück (oder nullopt bei Fehler)
  static std::optional<TokenPayload> verifyToken(const std::string &rawToken);
};

} // namespace utils
} // namespace rz
