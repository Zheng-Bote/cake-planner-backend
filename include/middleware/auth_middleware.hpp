/**
 * SPDX-FileComment: Auth Middleware
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file auth_middleware.hpp
 * @brief Auth Middleware
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include "crow.h"
#include "utils/token_utils.hpp"
#include <string>

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief middleware namespace.
 */
namespace middleware {

/**
 * @brief AuthMiddleware struct.
 */
struct AuthMiddleware {
  // Context stores data for the controller
  struct context {
    rz::utils::TokenPayload currentUser;
  };

  /**
   * @brief Function implementation.
   */
  void before_handle(crow::request &req, crow::response &res, context &ctx) {
    // 1. Whitelist
    std::string url = req.url;
    if (url == "/api/login" || url == "/api/auth/login" || url == "/api/auth/forgot-password" || url == "/api/register" || url == "/api/auth/register" || url == "/api/system/sysinfo" || url == "/api/system/healthcheck" ||
        url.starts_with("/static") || url.starts_with("/api/events/stream") ||
        url.starts_with("/api/uploads/")) {
      return;
    }

    // Always allow CORS Preflight
    if (req.method == crow::HTTPMethod::OPTIONS) {
        return;
    }

    // 2. Check Header
    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.empty() || !authHeader.starts_with("Bearer ")) {
      res.code = 401;
      res.body = "Unauthorized: Missing or invalid token format.";
      res.end();
      return;
    }

    // 3. Extract Token
    std::string token = authHeader.substr(7);

    // 4. Verify
    // CORRECTION: 'token' is already std::string, no conversion needed!
    auto payload = rz::utils::TokenUtils::verifyToken(token);

    if (!payload) {
      res.code = 403;
      res.body = "Forbidden: Invalid or expired token.";
      res.end();
      return;
    }

    // 5. Store user data in context
    ctx.currentUser = *payload;
  }

  /**
   * @brief Function implementation.
   */
  void after_handle(crow::request &req, crow::response &res, context &ctx) {
    // no-op
  }
};

} // namespace middleware
} // namespace rz
