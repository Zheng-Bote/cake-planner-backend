/**
 * @file event_model.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Unified Event Model
 * @version 0.3.9
 * @date 2026-01-04
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
    QString photoPath; // Haupt-Foto (Cover)

    // Dynamische Felder
    bool isOwner = false;
    bool isFuture = false;
    EventRating rating;

    // --- Methoden ---
    crow::json::wvalue toJson() const;

    bool create(const QString& userId);

    static std::vector<Event> getRange(const QString &start, const QString &end, const QString &userId);
    static std::optional<Event> getById(const QString& eventId, const QString& currentUserId);

    static bool deleteEvent(const QString& eventId, const QString& currentUserId);
    static bool rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment);

    // KORRIGIERT: Methode für Gallery Upload (DB Update)
    // Controller hat das File schon gespeichert, wir brauchen nur den Dateinamen.
    static bool uploadPhoto(const QString& eventId, const QString& userId, const QString& filename);

    std::string toIcsString() const;
};
