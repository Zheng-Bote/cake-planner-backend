/**
 * SPDX-FileComment: Auth Controller with Notification Service injection
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file auth_controller.hpp
 * @brief Auth Controller with Notification Service injection
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
#include "middleware/auth_middleware.hpp"

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
 * @brief AuthController class.
 */
class AuthController {
public:
    /**
     * @brief Construct a new Auth Controller
     * @param notifyService Pointer to the notification service
     */
    explicit AuthController(service::NotificationService* notifyService);

    /**
     * @brief Register authentication routes
     * @param app Crow App instance
     */
    // CORRECTED: Fully qualified namespace for AuthMiddleware
    void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app);

private:
    service::NotificationService* m_notifyService;
};

} // namespace controller
} // namespace rz
