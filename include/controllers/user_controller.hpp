/**
 * @file user_controller.hpp
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
#include "crow.h"
#include "middleware/auth_middleware.hpp" // WICHTIG: Middleware einbinden

class UserController {
public:
  // Wir akzeptieren jetzt spezifisch die App MIT AuthMiddleware
  static void registerRoutes(crow::App<AuthMiddleware> &app);
};