/**
 * @file app_helpers.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Application Helpers Header
 * @version 0.1.0
 * @date 2026-01-07
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "crow.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace rz::utils {

/**
 * @brief Ensures that a directory exists, creates it if necessary.
 * Exits the application on critical failure.
 */
inline void ensureDirectoryExists(const std::string& path) {
    if (path.empty()) return;
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
