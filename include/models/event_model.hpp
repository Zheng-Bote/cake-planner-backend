/**
 * @file event_model.hpp
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
#include "crow/json.h"
#include <QString>
#include <vector>

struct CakeEvent {
  QString id;
  QString date;      // YYYY-MM-DD
  QString bakerName; // Joined from users
  QString groupName; // Joined from groups
  QString description;
  QString photoPath;

  crow::json::wvalue toJson() const;

  // Gibt Events im Datumsbereich zurück (für alle Gruppen des Users)
  static std::vector<CakeEvent>
  getRange(const QString &start, const QString &end, const QString &userId);
  bool create(const QString &userId);
};
