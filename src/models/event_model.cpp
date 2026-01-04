/**
 * @file event_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Event Model Implementation
 * @version 0.3.10
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
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

crow::json::wvalue Event::toJson() const {
    crow::json::wvalue json;
    json["id"] = id.toStdString();
    json["groupId"] = groupId.toStdString();
    json["groupName"] = groupName.toStdString();
    json["bakerName"] = bakerName.toStdString();
    json["date"] = date.toStdString();
    json["description"] = description.toStdString();

    // Cover Bild (Falls leer -> Frontend nimmt Logo)
    json["photoUrl"] = photoPath.isEmpty() ? "" : "/api/uploads/" + photoPath.toStdString();

    // Galerie Array
    crow::json::wvalue galleryJson = crow::json::wvalue::list();
    for (size_t i = 0; i < gallery.size(); ++i) {
        crow::json::wvalue item;
        item["userName"] = gallery[i].userName.toStdString();
        item["url"] = gallery[i].photoUrl.toStdString();
        item["isMine"] = gallery[i].isMine;
        galleryJson[i] = std::move(item);
    }
    json["gallery"] = std::move(galleryJson);

    // Berechtigungen & Status
    json["isOwner"] = isOwner;
    json["canDelete"] = isOwner && isFuture;

    // Rating
    json["rating"]["average"] = rating.average;
    json["rating"]["count"] = rating.count;
    json["rating"]["myRating"] = rating.myRating;

    return json;
}

std::vector<Event> Event::getRange(const QString &start,
                                   const QString &end,
                                   const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<Event> events;

  // Wir holen das 'photo_path' aus der 'events' Tabelle (das Cover-Bild)
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
  // Speichert das Cover-Bild direkt in events
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

std::optional<Event> Event::getById(const QString& eventId, const QString& currentUserId) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // Lädt das Event inkl. dem Cover-Bild (photo_path)
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

    // Ratings laden
    QSqlQuery rateQuery(db);
    rateQuery.prepare("SELECT AVG(rating_value), COUNT(*) FROM ratings WHERE event_id = :eid");
    rateQuery.bindValue(":eid", eventId);
    if(rateQuery.exec() && rateQuery.next()) {
        e.rating.average = rateQuery.value(0).toDouble();
        e.rating.count = rateQuery.value(1).toInt();
    }

    // Mein Rating laden
    QSqlQuery myRateQuery(db);
    myRateQuery.prepare("SELECT rating_value FROM ratings WHERE event_id = :eid AND rater_id = :uid");
    myRateQuery.bindValue(":eid", eventId);
    myRateQuery.bindValue(":uid", currentUserId);
    if(myRateQuery.exec() && myRateQuery.next()) {
        e.rating.myRating = myRateQuery.value(0).toInt();
    }

    // NEU: Query 3 (Galerie)
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

// NEUE METHODE: Speichert Eintrag in event_photos und updatet optional events.photo_path
bool Event::uploadPhoto(const QString& eventId, const QString& userId, const QString& filename) {
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // 1. In event_photos Tabelle eintragen (1 Foto pro User per Event)
    // Wir nutzen ON CONFLICT DO UPDATE (Upsert)
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

    // 2. Prüfen, ob der User der Bäcker (Owner) ist
    QSqlQuery ownerQuery(db);
    ownerQuery.prepare("SELECT baker_id FROM events WHERE id = :id");
    ownerQuery.bindValue(":id", eventId);

    if (ownerQuery.exec() && ownerQuery.next()) {
        QString bakerId = ownerQuery.value("baker_id").toString();

        // 3. Wenn ja, aktualisieren wir das Cover-Bild in der events-Tabelle
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
