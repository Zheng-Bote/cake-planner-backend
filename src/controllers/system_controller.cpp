/**
 * @file system_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Implementation of system routes
 * @version 0.1.0
 * @date 2026-01-07
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/system_controller.hpp"
#include "rz_config.hpp"
#include <QDateTime>
#include <string>

namespace rz::controller {

void SystemController::registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app) {

    // 1. SysInfo - Returns complete Build/Config Info from rz_config.hpp
    CROW_ROUTE(app, "/api/system/sysinfo")
    ([]() {
        crow::json::wvalue info;

        // Strings (Expliziter Cast von std::string_view zu std::string notwendig)
        info["project_name"] = std::string(rz::config::PROJECT_NAME);
        info["prog_longname"] = std::string(rz::config::PROG_LONGNAME);
        info["description"] = std::string(rz::config::PROJECT_DESCRIPTION);
        info["executable"] = std::string(rz::config::EXECUTABLE_NAME);

        info["version"] = std::string(rz::config::VERSION);

        // Integers (Crow wvalue can handle int directly)
        info["version_major"] = rz::config::PROJECT_VERSION_MAJOR;
        info["version_minor"] = rz::config::PROJECT_VERSION_MINOR;
        info["version_patch"] = rz::config::PROJECT_VERSION_PATCH;

        info["homepage"] = std::string(rz::config::PROJECT_HOMEPAGE_URL);
        info["author"] = std::string(rz::config::AUTHOR);
        info["created_year"] = std::string(rz::config::CREATED_YEAR);
        info["organization"] = std::string(rz::config::ORGANIZATION);
        info["domain"] = std::string(rz::config::DOMAIN);

        // Compiler & Framework Infos
        info["cxx_standard"] = std::string(rz::config::CMAKE_CXX_STANDARD);
        info["compiler"] = std::string(rz::config::CMAKE_CXX_COMPILER);
        info["qt_version"] = std::string(rz::config::QT_VERSION_BUILD);

        return info;
    });

    // 2. Healthcheck - Returns Status UP & Timestamp
    CROW_ROUTE(app, "/api/system/healthcheck")
    ([]() {
        crow::json::wvalue status;
        status["status"] = "UP";
        status["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
        return status;
    });
}

} // namespace rz::controller
