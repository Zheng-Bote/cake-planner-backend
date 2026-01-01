/**
 * @file env_loader.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>

class EnvLoader {
public:
  static void load(const QString &filename = "CakePlanner.env");
  static QString get(const QString &key, const QString &defaultValue = "");
  static int getInt(const QString &key, int defaultValue);
};