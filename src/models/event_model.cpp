/**
 * @file event_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Event Model Implementation
 * @version 0.4.1
 * @date 2026-01-22
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "models/event_model.hpp"
#include "database.hpp"

#include <QSqlQuery>
#include <QUuid>
#include <QVariant>
#include <QSqlError>
#include <QDate>
#include <QDir>
#include <QDebug>
#include <sstream>

// --- Methods ---

/**
 * @brief Converts the Event object to a JSON value.
 *
 * @return A crow::json::wvalue containing the event data.
 */
crow::json::wvalue Event::toJson() const {
    crow::json::wvalue json;
    json["id"] = id.toStdString();
    json["groupId"] = groupId.toStdString();
    json["groupName"] = groupName.toStdString();
    json["bakerName"] = bakerName.toStdString();
    json["date"] = date.toStdString();
    json["description"] = description.toStdString();

    // Cover Image (If empty -> Frontend takes logo)
    json["photoUrl"] = photoPath.isEmpty() ? "" : "/api/uploads/" + photoPath.toStdString();

    // Gallery Array
    crow::json::wvalue galleryJson = crow::json::wvalue::list();
    for (size_t i = 0; i < gallery.size(); ++i) {
        crow::json::wvalue item;
        item["userName"] = gallery[i].userName.toStdString();
        item["url"] = gallery[i].photoUrl.toStdString();
        item["isMine"] = gallery[i].isMine;
        galleryJson[i] = std::move(item);
    }
    json["gallery"] = std::move(galleryJson);

    // Permissions & Status
    json["isOwner"] = isOwner;
    json["canDelete"] = isOwner && isFuture;

    // Rating
    json["rating"]["average"] = rating.average;
    json["rating"]["count"] = rating.count;
    json["rating"]["myRating"] = rating.myRating;

    return json;
}

/**
 * @brief Retrieves a list of events within a specific date range for a user.
 *
 * @param start The start date (inclusive, format yyyy-MM-dd).
 * @param end The end date (inclusive, format yyyy-MM-dd).
 * @param userId The ID of the user requesting the events.
 * @return A vector of Event objects.
 */
std::vector<Event> Event::getRange(const QString &start,
                                   const QString &end,
                                   const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<Event> events;

  // We fetch 'photo_path' from 'events' table (the cover image)
  QString sql = R"(
        SELECT e.id, e.event_date, e.description, e.photo_path, e.group_id,
               u.full_name, u.id as baker_id, g.name as group_name
        FROM events e
        JOIN users u ON e.baker_id = u.id
        JOIN groups g ON e.group_id = g.id
        JOIN group_members gm ON e.group_id = gm.group_id
        WHERE gm.user_id = :userId
          AND e.event_date >= :start
          AND e.event_date <= :end
        ORDER BY e.event_date ASC
    )";

  query.prepare(sql);
  query.bindValue(":userId", userId);
  query.bindValue(":start", start);
  query.bindValue(":end", end);

  if (query.exec()) {
    while (query.next()) {
      Event e;
      e.id = query.value("id").toString();
      e.date = query.value("event_date").toString();
      e.bakerId = query.value("baker_id").toString();
      e.bakerName = query.value("full_name").toString();
      e.groupId = query.value("group_id").toString();
      e.groupName = query.value("group_name").toString();
      e.description = query.value("description").toString();
      e.photoPath = query.value("photo_path").toString();

      e.isOwner = (e.bakerId == userId);
      e.isFuture = (QDate::fromString(e.date, "yyyy-MM-dd") >= QDate::currentDate());

      events.push_back(e);
    }
  }
  return events;
}

/**
 * @brief Creates a new event in the database.
 *
 * @param userId The ID of the user creating the event (the baker).
 * @return True if successful, false otherwise.
 */
bool Event::create(const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();

  if (this->id.isEmpty()) {
    this->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }

  QSqlQuery userQuery(db);
  userQuery.prepare(R"(
    SELECT u.full_name, gm.group_id, g.name as group_name
    FROM users u
    JOIN group_members gm ON u.id = gm.user_id
    JOIN groups g ON gm.group_id = g.id
    WHERE u.id = :uid LIMIT 1
  )");
  userQuery.bindValue(":uid", userId);

  if (userQuery.exec() && userQuery.next()) {
    this->bakerName = userQuery.value("full_name").toString();
    this->groupId = userQuery.value("group_id").toString();
    this->groupName = userQuery.value("group_name").toString();
    this->bakerId = userId;
  } else {
    return false;
  }

  QSqlQuery query(db);
  // Saves cover image directly in events
  query.prepare("INSERT INTO events (id, group_id, baker_id, event_date, "
                "description, photo_path) "
                "VALUES (:id, :gid, :bid, :date, :desc, :photo)");

  query.bindValue(":id", this->id);
  query.bindValue(":gid", this->groupId);
  query.bindValue(":bid", userId);
  query.bindValue(":date", this->date);
  query.bindValue(":desc", this->description);
  query.bindValue(":photo", this->photoPath);

  return query.exec();
}

/**
 * @brief Retrieves a specific event by its ID.
 *
 * @param eventId The ID of the event to retrieve.
 * @param currentUserId The ID of the currently logged-in user.
 * @return An optional Event object if found, std::nullopt otherwise.
 */
std::optional<Event> Event::getById(const QString& eventId, const QString& currentUserId) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // Loads event incl. cover image (photo_path)
    query.prepare(R"(
        SELECT e.*, u.full_name, g.name as group_name
        FROM events e
        JOIN users u ON e.baker_id = u.id
        JOIN groups g ON e.group_id = g.id
        WHERE e.id = :id
    )");
    query.bindValue(":id", eventId);

    if (!query.exec() || !query.next()) return std::nullopt;

    Event e;
    e.id = query.value("id").toString();
    e.groupId = query.value("group_id").toString();
    e.groupName = query.value("group_name").toString();
    e.bakerId = query.value("baker_id").toString();
    e.bakerName = query.value("full_name").toString();
    e.date = query.value("event_date").toString();
    e.description = query.value("description").toString();
    e.photoPath = query.value("photo_path").toString();

    e.isOwner = (e.bakerId == currentUserId);
    e.isFuture = (QDate::fromString(e.date, "yyyy-MM-dd") >= QDate::currentDate());

    // Load Ratings
    QSqlQuery rateQuery(db);
    rateQuery.prepare("SELECT AVG(rating_value), COUNT(*) FROM ratings WHERE event_id = :eid");
    rateQuery.bindValue(":eid", eventId);
    if(rateQuery.exec() && rateQuery.next()) {
        e.rating.average = rateQuery.value(0).toDouble();
        e.rating.count = rateQuery.value(1).toInt();
    }

    // Load my rating
    QSqlQuery myRateQuery(db);
    myRateQuery.prepare("SELECT rating_value FROM ratings WHERE event_id = :eid AND rater_id = :uid");
    myRateQuery.bindValue(":eid", eventId);
    myRateQuery.bindValue(":uid", currentUserId);
    if(myRateQuery.exec() && myRateQuery.next()) {
        e.rating.myRating = myRateQuery.value(0).toInt();
    }

    // NEW: Query 3 (Gallery)
    QSqlQuery galleryQuery(db);
    galleryQuery.prepare(R"(
        SELECT ep.photo_path, u.full_name, ep.user_id
        FROM event_photos ep
        JOIN users u ON ep.user_id = u.id
        WHERE ep.event_id = :eid
        ORDER BY ep.uploaded_at DESC
    )");
    galleryQuery.bindValue(":eid", eventId);

    if (galleryQuery.exec()) {
        while (galleryQuery.next()) {
            GalleryItem item;
            QString path = galleryQuery.value("photo_path").toString();
            item.photoUrl = "/api/uploads/" + path;
            item.userName = galleryQuery.value("full_name").toString();
            item.userId = galleryQuery.value("user_id").toString();
            item.isMine = (item.userId == currentUserId);
            e.gallery.push_back(item);
        }
    }

    return e;
}

/**
 * @brief Deletes an event.
 *
 * Only the owner can delete their own future events.
 *
 * @param eventId The ID of the event to delete.
 * @param currentUserId The ID of the user attempting to delete the event.
 * @return True if successful and authorized, false otherwise.
 */
bool Event::deleteEvent(const QString& eventId, const QString& currentUserId) {
    auto evt = getById(eventId, currentUserId);
    if (!evt) return false;

    if (!evt->isOwner || !evt->isFuture) return false;

    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare("DELETE FROM events WHERE id = :id");
    query.bindValue(":id", eventId);
    return query.exec();
}

/**
 * @brief Rates an event.
 *
 * @param eventId The ID of the event to rate.
 * @param userId The ID of the user submitting the rating.
 * @param stars The number of stars (rating value).
 * @param comment An optional comment.
 * @return True if successful, false otherwise.
 */
bool Event::rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO ratings (id, event_id, rater_id, rating_value, comment)
        VALUES (:id, :eid, :uid, :val, :comment)
        ON CONFLICT(event_id, rater_id) DO UPDATE SET
            rating_value = excluded.rating_value,
            comment = excluded.comment
    )");

    query.bindValue(":id", QUuid::createUuid().toString(QUuid::WithoutBraces));
    query.bindValue(":eid", eventId);
    query.bindValue(":uid", userId);
    query.bindValue(":val", stars);
    query.bindValue(":comment", comment);

    return query.exec();
}

/**
 * @brief Uploads a photo to an event's gallery.
 *
 * If the uploader is the baker (owner), the event's cover photo is also updated.
 *
 * @param eventId The ID of the event.
 * @param userId The ID of the user uploading the photo.
 * @param filename The relative path/filename of the uploaded photo.
 * @return True if successful, false otherwise.
 */
bool Event::uploadPhoto(const QString& eventId, const QString& userId, const QString& filename) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // 1. Insert into event_photos table (1 photo per user per event)
    // We use ON CONFLICT DO UPDATE (Upsert)
    query.prepare(R"(
        INSERT INTO event_photos (event_id, user_id, photo_path, uploaded_at)
        VALUES (:eid, :uid, :path, CURRENT_TIMESTAMP)
        ON CONFLICT(event_id, user_id) DO UPDATE SET
            photo_path = excluded.photo_path,
            uploaded_at = CURRENT_TIMESTAMP
    )");
    query.bindValue(":eid", eventId);
    query.bindValue(":uid", userId);
    query.bindValue(":path", filename);

    if (!query.exec()) {
        qDebug() << "SQL Error uploadPhoto:" << query.lastError().text();
        return false;
    }

    // 2. Check if user is the baker (owner)
    QSqlQuery ownerQuery(db);
    ownerQuery.prepare("SELECT baker_id FROM events WHERE id = :id");
    ownerQuery.bindValue(":id", eventId);

    if (ownerQuery.exec() && ownerQuery.next()) {
        QString bakerId = ownerQuery.value("baker_id").toString();

        // 3. If yes, we update the cover image in the events table
        if (bakerId == userId) {
            QSqlQuery updateCover(db);
            updateCover.prepare("UPDATE events SET photo_path = :path WHERE id = :id");
            updateCover.bindValue(":path", filename);
            updateCover.bindValue(":id", eventId);
            if (!updateCover.exec()) {
                 qDebug() << "SQL Error updateCover:" << updateCover.lastError().text();
            }
        }
    }

    return true;
}

/**
 * @brief Generates an ICS (iCalendar) string for the event.
 *
 * @return The ICS string.
 */
std::string Event::toIcsString() const {
    std::stringstream ss;
    ss << "BEGIN:VCALENDAR\r\n"
       << "VERSION:2.0\r\n"
       << "PRODID:-//CakePlanner//DE\r\n"
       << "BEGIN:VEVENT\r\n"
       << "UID:" << id.toStdString() << "\r\n"
       << "DTSTAMP:" << QDateTime::currentDateTimeUtc().toString("yyyyMMdd'T'HHmmss'Z'").toStdString() << "\r\n"
       << "DTSTART;VALUE=DATE:" << QDate::fromString(date, "yyyy-MM-dd").toString("yyyyMMdd").toStdString() << "\r\n"
       << "SUMMARY:Kuchen: " << bakerName.toStdString() << "\r\n"
       << "DESCRIPTION:" << description.toStdString() << "\r\n"
       << "END:VEVENT\r\n"
       << "END:VCALENDAR\r\n";
    return ss.str();
}

/**
 * @brief Retrieves a list of events ranked by their average rating.
 *
 * @param userId The ID of the requesting user (used for group filtering).
 * @param limit The maximum number of events to return.
 * @return A vector of ranked Event objects.
 */
std::vector<Event> Event::getRanked(const QString& userId, int limit) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // We join ratings, calculate average, and sort by it
    // COALESCE(AVG(...), 0) ensures events without rating are valued as 0
    QString sql = R"(
        SELECT e.id, e.event_date, e.description, e.photo_path, e.group_id,
               u.full_name, u.id as baker_id, g.name as group_name,
               COALESCE(AVG(r.rating_value), 0) as avg_rating
        FROM events e
        JOIN users u ON e.baker_id = u.id
        JOIN groups g ON e.group_id = g.id
        JOIN group_members gm ON e.group_id = gm.group_id
        LEFT JOIN ratings r ON e.id = r.event_id
        WHERE gm.user_id = :userId
        GROUP BY e.id
        ORDER BY avg_rating DESC, e.event_date DESC
        LIMIT :limit
    )";

    query.prepare(sql);
    query.bindValue(":userId", userId);
    query.bindValue(":limit", limit);

    std::vector<Event> events;

    if (query.exec()) {
        while (query.next()) {
            Event e;
            // ... map base data (id, date, bakerName etc.) as in getRange ...
            e.id = query.value("id").toString();
            e.bakerName = query.value("full_name").toString();
            e.date = query.value("event_date").toString();
            e.description = query.value("description").toString();
            e.photoPath = query.value("photo_path").toString();
            e.rating.average = query.value("avg_rating").toDouble(); // The sorted rating

            QSqlQuery galQuery(db);
            galQuery.prepare(R"(
                SELECT ep.photo_path, ep.user_id, u.full_name
                FROM event_photos ep
                LEFT JOIN users u ON ep.user_id = u.id
                WHERE ep.event_id = :eid
            )");
            galQuery.bindValue(":eid", e.id);
            if(galQuery.exec()) {
                while(galQuery.next()){
                    GalleryItem item;
                    item.photoUrl = "/api/uploads/" + galQuery.value("photo_path").toString();
                    item.userId = galQuery.value("user_id").toString();
                    item.userName = galQuery.value("full_name").toString();
                    // Check isMine simply here
                    item.isMine = (item.userId == userId);
                    e.gallery.push_back(item);
                }
            }

            // Integrate cover image into gallery logic too, if desired?
            // Currently e.gallery is only community photos.

            events.push_back(e);
        }
    }
    return events;
}
