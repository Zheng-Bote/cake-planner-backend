/**
 * @file event_controller.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Event Controller Header
 * @version 0.3.4
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

class EventController {
public:
  static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app, service::NotificationService* notifyService);
};

} // namespace controller
} // namespace rz
