/**
 * @file env_loader.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Environment Loader Header
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>
#include <string>

// Namespace rz::utils
namespace rz {
namespace utils {

class EnvLoader {
public:
    static void load(const std::string& path);

    // Existing Qt-style getter
    static QString get(const std::string& key, const std::string& defaultVal = "");

    // NEW: std::string getter for main.cpp and controllers
    static std::string getString(const std::string& key, const std::string& defaultVal = "");

    static int getInt(const std::string& key, int defaultVal = 0);
};

} // namespace utils
} // namespace rz
