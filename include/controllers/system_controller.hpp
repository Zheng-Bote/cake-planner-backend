/**
 * @file system_controller.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Controller for system related routes (Healthcheck, Sysinfo)
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "crow.h"
#include "middleware/auth_middleware.hpp"

namespace rz::controller {

class SystemController {
public:
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app);
};

} // namespace rz::controller
