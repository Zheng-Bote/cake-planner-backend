#include "models/event_model.hpp"
#include "database.hpp"
#include <QSqlQuery>
#include <QUuid> // Wichtig für createUuid
#include <QVariant>

crow::json::wvalue CakeEvent::toJson() const {
  crow::json::wvalue json;
  json["id"] = id.toStdString();
  json["date"] = date.toStdString();
  json["bakerName"] = bakerName.toStdString();
  json["description"] = description.toStdString();

  // Falls ein Foto existiert, URL generieren
  if (!photoPath.isEmpty()) {
    json["photoUrl"] = ("/static/uploads/" + photoPath).toStdString();
  }

  return json;
}

std::vector<CakeEvent> CakeEvent::getRange(const QString &start,
                                           const QString &end,
                                           const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<CakeEvent> events;

  // Wir holen Events inkl. photo_path
  QString sql = R"(
        SELECT e.id, e.event_date, e.description, e.photo_path, u.full_name, g.name as group_name
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
      CakeEvent e;
      e.id = query.value("id").toString();
      e.date = query.value("event_date").toString();
      e.bakerName = query.value("full_name").toString();
      e.groupName = query.value("group_name").toString();
      e.description = query.value("description").toString();
      // Hier auch beim Laden setzen:
      e.photoPath = query.value("photo_path").toString();

      events.push_back(e);
    }
  }
  return events;
}

bool CakeEvent::create(const QString &userId) {
  auto db = DatabaseManager::instance().getDatabase();

  if (this->id.isEmpty()) {
    this->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }

  // Gruppe des Users finden
  QSqlQuery groupQuery(db);
  groupQuery.prepare(
      "SELECT group_id FROM group_members WHERE user_id = :uid LIMIT 1");
  groupQuery.bindValue(":uid", userId);

  QString groupId;
  if (groupQuery.exec() && groupQuery.next()) {
    groupId = groupQuery.value(0).toString();
  } else {
    return false;
  }

  QSqlQuery query(db);
  // WICHTIG: photo_path im INSERT
  query.prepare("INSERT INTO events (id, group_id, baker_id, event_date, "
                "description, photo_path) "
                "VALUES (:id, :gid, :bid, :date, :desc, :photo)");

  query.bindValue(":id", this->id);
  query.bindValue(":gid", groupId);
  query.bindValue(":bid", userId);
  query.bindValue(":date", this->date);
  query.bindValue(":desc", this->description);
  query.bindValue(":photo", this->photoPath); // Hier wird das Feld genutzt

  return query.exec();
}