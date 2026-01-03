/**
 * @file env_loader.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.2.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>

// Namespace rz::utils
namespace rz {
namespace utils {

class EnvLoader {
public:
    static void load(const std::string& path);
    static QString get(const std::string& key, const std::string& defaultVal = "");
    static int getInt(const std::string& key, int defaultVal = 0);
};

} // namespace utils
} // namespace rz
