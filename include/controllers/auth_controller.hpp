/**
 * @file auth_controller.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Auth Controller with Notification Service injection
 * @version 0.2.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp"

// Forward Declaration
namespace rz {
namespace service {
    class NotificationService;
}
}

// Namespace rz::controller
namespace rz {
namespace controller {

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
    // KORRIGIERT: Voll qualifizierter Namespace für AuthMiddleware
    void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app);

private:
    service::NotificationService* m_notifyService;
};

} // namespace controller
} // namespace rz
