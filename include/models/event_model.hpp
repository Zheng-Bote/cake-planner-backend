/**
 * @file event_model.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Unified Event Model
 * @version 0.3.0
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include "crow/json.h"
#include <QString>
#include <vector>
#include <optional>
#include <string>

struct EventRating {
    double average = 0.0;
    int count = 0;
    int myRating = 0;
};

struct Event {
    QString id;
    QString groupId;
    QString groupName;
    QString bakerId;
    QString bakerName;
    QString date;      // YYYY-MM-DD
    QString description;
    QString photoPath;

    // Dynamische Felder
    bool isOwner = false;
    bool isFuture = false;
    EventRating rating;

    // --- Methoden ---
    crow::json::wvalue toJson() const;

    // Core Actions
    bool create(const QString& userId);

    // Static Fetchers
    static std::vector<Event> getRange(const QString &start, const QString &end, const QString &userId);
    static std::optional<Event> getById(const QString& eventId, const QString& currentUserId);

    // Actions
    static bool deleteEvent(const QString& eventId, const QString& currentUserId);
    static bool rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment);

    // Foto Upload
    static bool uploadPhoto(const QString& eventId, const QString& userId, const std::string& fileContent, const std::string& ext);

    // ICS
    std::string toIcsString() const;
};
