/**
 * SPDX-FileComment: Admin Controller
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file admin_controller.hpp
 * @brief Admin Controller
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
#include "middleware/auth_middleware.hpp" // IMPORTANT: Include middleware

// Forward Declaration
namespace rz {
/**
 * @brief service namespace.
 */
namespace service {
    class NotificationService;
}
}

// Namespace rz::controller
namespace rz {
/**
 * @brief controller namespace.
 */
namespace controller {

/**
 * @brief AdminController class.
 */
class AdminController {
public:
  // Signature update: Now accepts NotificationService*
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app, service::NotificationService* notifyService);
};

} // namespace controller
} // namespace rz
