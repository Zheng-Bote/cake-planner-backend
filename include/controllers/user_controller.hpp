/**
 * @file user_controller.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Controller with Email Notifications
 * @version 0.2.1
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp" // WICHTIG: Middleware einbinden

// Forward Declaration
namespace rz {
namespace service {
    class NotificationService;
}
}

// Namespace rz::controller
namespace rz {
namespace controller {

class UserController {
public:
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app, service::NotificationService* notifyService);
};

} // namespace controller
} // namespace rz
