/**
 * @file auth_controller.hpp
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
#include "middleware/auth_middleware.hpp"

class AuthController {
public:
  // Wir übergeben die App als Template oder Referenz
  // Da wir Middleware nutzen, müssen wir den App-Typ kennen
  static void registerRoutes(crow::App<AuthMiddleware> &app);
};