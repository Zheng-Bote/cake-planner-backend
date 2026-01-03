/**
 * @file user_controller.hpp
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
#include "crow.h"
#include "middleware/auth_middleware.hpp" // WICHTIG: Middleware einbinden

// Namespace rz::controller
namespace rz {
namespace controller {

class UserController {
public:
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app);
};

} // namespace controller
} // namespace rz
