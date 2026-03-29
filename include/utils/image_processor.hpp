/**
 * SPDX-FileComment: Image Processor Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file image_processor.hpp
 * @brief Image Processor Header
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
#include <vector>

/**
 * @brief ImageProcessor class.
 */
class ImageProcessor {
public:
    /**
     * @brief Function implementation.
     */
    static void generateWebPVersions(const QString& sourcePath);
    /**
     * @brief Function implementation.
     */
    static void deleteAllVersions(const QString& sourcePath);

private:
    static const std::vector<int> TARGET_WIDTHS;
};
