/**
 * SPDX-FileComment: Environment Loader Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file env_loader.hpp
 * @brief Environment Loader Header
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include <QString>
#include <string>

// Namespace rz::utils
namespace rz {
/**
 * @brief utils namespace.
 */
namespace utils {

/**
 * @brief EnvLoader class.
 */
class EnvLoader {
public:
    /**
     * @brief Function implementation.
     */
    static void load(const std::string& path);

    // Existing Qt-style getter
    static QString get(const std::string& key, const std::string& defaultVal = "");

    // NEW: std::string getter for main.cpp and controllers
    static std::string getString(const std::string& key, const std::string& defaultVal = "");

    /**
     * @brief Function implementation.
     */
    static int getInt(const std::string& key, int defaultVal = 0);
};

} // namespace utils
} // namespace rz
