/**
 * @file image_processor.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Image Processor Header
 * @version 0.1.1
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>
#include <vector>

class ImageProcessor {
public:
    static void generateWebPVersions(const QString& sourcePath);
    static void deleteAllVersions(const QString& sourcePath);

private:
    static const std::vector<int> TARGET_WIDTHS;
};
