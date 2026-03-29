/**
 * SPDX-FileComment: Unified Event Model
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file event_model.hpp
 * @brief Unified Event Model
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include "crow/json.h"
#include <QString>
#include <vector>
#include <optional>
#include <string>

/**
 * @brief EventRating struct.
 */
struct EventRating {
    double average = 0.0;
    int count = 0;
    int myRating = 0;
};

// Structure for gallery items
struct GalleryItem {
    QString userId;
    QString userName;
    QString photoUrl;
    bool isMine;
};

/**
 * @brief Event struct.
 */
struct Event {
    QString id;
    QString groupId;
    QString groupName;
    QString bakerId;
    QString bakerName;
    QString date;      // YYYY-MM-DD
    QString description;
    QString photoPath; // Main photo (Cover)

    // Dynamic fields
    bool isOwner = false;
    bool isFuture = false;
    EventRating rating;
    std::vector<GalleryItem> gallery;

    // --- Methods ---
    crow::json::wvalue toJson() const;

    /**
     * @brief Function implementation.
     */
    bool create(const QString& userId);

    /**
     * @brief Function implementation.
     */
    static std::vector<Event> getRange(const QString &start, const QString &end, const QString &userId);
    /**
     * @brief Function implementation.
     */
    static std::optional<Event> getById(const QString& eventId, const QString& currentUserId);

    /**
     * @brief Function implementation.
     */
    static bool deleteEvent(const QString& eventId, const QString& currentUserId);
    /**
     * @brief Function implementation.
     */
    static bool rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment);

    // Method for gallery upload (DB Update)
    // Controller has already saved the file, we only need the filename.
    static bool uploadPhoto(const QString& eventId, const QString& userId, const QString& filename);

    /**
     * @brief Function implementation.
     */
    static std::vector<Event> getRanked(const QString& userId, int limit = 50);

    /**
     * @brief Function implementation.
     */
    std::string toIcsString() const;
};
