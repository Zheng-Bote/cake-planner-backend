/**
 * SPDX-FileComment: Application Helpers Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file app_helpers.hpp
 * @brief Application Helpers Header
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
#include <filesystem>
#include <iostream>
#include <string>

/**
 * @brief rz namespace.
 */
namespace rz::utils {

/**
 * @brief Ensures that a directory exists, creates it if necessary.
 * Exits the application on critical failure.
 */
inline void ensureDirectoryExists(const std::string& path) {
    if (path.empty()) return;
    /**
     * @brief fs namespace.
     */
    namespace fs = std::filesystem;
    if (!fs::exists(path)) {
        try {
            fs::create_directories(path);
        } catch (const std::exception& e) {
            std::cerr << "CRITICAL: Cannot create directory: " << path << " - " << e.what() << std::endl;
            exit(1);
        }
    }
}

/**
 * @brief Creates a standardized JSON error response object.
 */
inline crow::json::wvalue makeErrorJson(int code, const std::string& message) {
    crow::json::wvalue x;
    x["error"] = true;
    x["status"] = code;
    x["message"] = message;
    return x;
}

} // namespace rz::utils
