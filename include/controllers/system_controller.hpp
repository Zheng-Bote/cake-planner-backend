/**
 * SPDX-FileComment: Controller for system related routes (Healthcheck, Sysinfo)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file system_controller.hpp
 * @brief Controller for system related routes (Healthcheck, Sysinfo)
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

/**
 * @brief rz namespace.
 */
namespace rz::controller {

/**
 * @brief SystemController class.
 */
class SystemController {
public:
    /**
     * @brief Function implementation.
     */
    static void registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app);
};

} // namespace rz::controller
