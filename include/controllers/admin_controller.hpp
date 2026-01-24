/**
 * @file admin_controller.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Admin Controller
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp" // IMPORTANT: Include middleware

// Forward Declaration
namespace rz {
namespace service {
    class NotificationService;
}
}

// Namespace rz::controller
namespace rz {
namespace controller {

class AdminController {
public:
  // Signature update: Now accepts NotificationService*
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app, service::NotificationService* notifyService);
};

} // namespace controller
} // namespace rz
