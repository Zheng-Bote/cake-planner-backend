#include "models/user_model.hpp"
#include "database.hpp"
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

crow::json::wvalue User::toJson() const {
  crow::json::wvalue json;
  json["id"] = id.toStdString();
  json["name"] = full_name.toStdString();
  json["email"] = email.toStdString();
  json["is_active"] = is_active;
  // Password Hash und TOTP Secret geben wir nicht raus
  return json;
}

std::optional<User> User::getByEmail(const QString &email) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT id, full_name, email, password_hash, is_active, "
                "is_admin FROM users WHERE email = :email");
  query.bindValue(":email", email);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();
    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    return u;
  }
  return std::nullopt;
}

std::vector<User> User::getAll() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<User> users;

  if (query.exec(
          "SELECT id, full_name, email, is_active, is_admin FROM users")) {
    while (query.next()) {
      User u;
      u.id = query.value("id").toString();
      u.full_name = query.value("full_name").toString();
      u.email = query.value("email").toString();
      u.is_active = query.value("is_active").toBool();
      u.is_admin = query.value("is_admin").toBool();
      users.push_back(u);
    }
  }
  return users;
}

// Hier ist die Implementierung für den Seeder
bool User::existsAnyAdmin() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT COUNT(*) FROM users WHERE is_admin = 1");
  if (query.exec() && query.next()) {
    return query.value(0).toInt() > 0;
  }
  return false;
}

bool User::create() {
  auto db = DatabaseManager::instance().getDatabase();

  if (this->id.isEmpty()) {
    this->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
  }

  QSqlQuery query(db);
  query.prepare("INSERT INTO users (id, full_name, email, password_hash, "
                "is_active, is_admin) "
                "VALUES (:id, :name, :email, :pass, :active, :admin)");
  query.bindValue(":id", this->id);
  query.bindValue(":name", this->full_name);
  query.bindValue(":email", this->email);
  query.bindValue(":pass", this->password_hash);
  query.bindValue(":active", this->is_active ? 1 : 0);
  query.bindValue(":admin", this->is_admin ? 1 : 0);

  return query.exec();
}