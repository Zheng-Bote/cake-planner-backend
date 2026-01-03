/**
 * @file auth_middleware.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Auth Middleware
 * @version 0.2.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow.h"
#include "utils/token_utils.hpp"
#include <string>

namespace rz {
namespace middleware {

struct AuthMiddleware {
  // Kontext speichert Daten für den Controller
  struct context {
    rz::utils::TokenPayload currentUser;
  };

  void before_handle(crow::request &req, crow::response &res, context &ctx) {
    // 1. Whitelist
    std::string url = req.url;
    if (url == "/api/login" || url == "/api/register" || url == "/api/status" ||
        url.starts_with("/static")) {
      return;
    }

    // 2. Header prüfen
    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.empty() || !authHeader.starts_with("Bearer ")) {
      res.code = 401;
      res.body = "Unauthorized: Missing or invalid token format.";
      res.end();
      return;
    }

    // 3. Token extrahieren
    std::string token = authHeader.substr(7);

    // 4. Verifizieren
    // KORREKTUR: 'token' ist bereits std::string, keine Konvertierung nötig!
    auto payload = rz::utils::TokenUtils::verifyToken(token);

    if (!payload) {
      res.code = 403;
      res.body = "Forbidden: Invalid or expired token.";
      res.end();
      return;
    }

    // 5. User-Daten im Kontext speichern
    ctx.currentUser = *payload;
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx) {
    // no-op
  }
};

} // namespace middleware
} // namespace rz
