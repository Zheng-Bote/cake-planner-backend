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
  // WICHTIG: Hier muss es matchen!
  json["isAdmin"] = is_admin;   // Nicht "is_admin"
  json["isActive"] = is_active; // Nicht "is_active"
  json["mustChangePassword"] = must_change_password;
  // Password Hash und TOTP Secret geben wir nicht raus
  json["has2FA"] = !totp_secret.isEmpty();
  json["groupId"] = groupId.toStdString();
  return json;
}

std::optional<User> User::getByEmail(const QString &email) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("SELECT id, full_name, email, password_hash, is_active, "
                "is_admin, totp_secret, must_change_password FROM users WHERE "
                "email = :email");
  query.bindValue(":email", email);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();
    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();
    return u;
  }
  return std::nullopt;
}

std::optional<User> User::getById(const QString &id) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare(
      "SELECT id, full_name, email, password_hash, is_active, "
      "is_admin, totp_secret, must_change_password FROM users WHERE id = :id");
  query.bindValue(":id", id);

  if (query.exec() && query.next()) {
    User u;
    u.id = query.value("id").toString();
    u.full_name = query.value("full_name").toString();
    u.email = query.value("email").toString();
    u.password_hash = query.value("password_hash").toString();
    u.is_active = query.value("is_active").toBool();
    u.is_admin = query.value("is_admin").toBool();
    u.must_change_password = query.value("must_change_password").toBool();
    u.totp_secret = query.value("totp_secret").toString();
    return u;
  }
  return std::nullopt;
}

std::vector<User> User::getAll() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<User> users;

  query.prepare(R"(
        SELECT u.id, u.full_name, u.email, u.is_active, u.is_admin, u.must_change_password,
               gm.group_id 
        FROM users u
        LEFT JOIN group_members gm ON u.id = gm.user_id
    )");

  if (query.exec()) {
    while (query.next()) {
      User u;
      u.id = query.value("id").toString();
      u.full_name = query.value("full_name").toString();
      u.email = query.value("email").toString();
      u.is_active = query.value("is_active").toBool();
      u.is_admin = query.value("is_admin").toBool();
      u.must_change_password = query.value("must_change_password").toBool();
      u.groupId = query.value("group_id").toString();
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

bool User::enable2FA(const QString &secret) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET totp_secret = :secret WHERE id = :id");
  query.bindValue(":secret", secret);
  query.bindValue(":id", this->id);

  if (query.exec()) {
    this->totp_secret = secret; // Lokal updaten
    return true;
  }
  return false;
}

bool User::updateStatus(const QString &userId, bool isActive) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  query.prepare("UPDATE users SET is_active = :active WHERE id = :id");
  query.bindValue(":active", isActive);
  query.bindValue(":id", userId);

  return query.exec();
}

bool User::setMustChangePassword(const QString &userId, bool mustChange) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET must_change_password = :val WHERE id = :id");
  query.bindValue(":val", mustChange ? 1 : 0);
  query.bindValue(":id", userId);
  return query.exec();
}

bool User::updatePassword(const QString &userId, const QString &newHash) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  // LOGIK: Neues Passwort setzen UND Flag löschen
  query.prepare("UPDATE users SET password_hash = :hash, must_change_password "
                "= 0 WHERE id = :id");
  query.bindValue(":hash", newHash);
  query.bindValue(":id", userId);

  return query.exec();
}

// getAllGroups Implementierung
std::vector<std::pair<QString, QString>> User::getAllGroups() {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);
  std::vector<std::pair<QString, QString>> groups;

  if (query.exec("SELECT id, name FROM groups")) {
    while (query.next()) {
      groups.push_back(
          {query.value("id").toString(), query.value("name").toString()});
    }
  }
  return groups;
}

// assignToGroup Implementierung
bool User::assignToGroup(const QString &userId, const QString &groupId) {
  auto db = DatabaseManager::instance().getDatabase();
  QSqlQuery query(db);

  // Zuerst: Alte Zuweisungen löschen (Wir erlauben erstmal nur 1 Gruppe pro
  // User)
  query.prepare("DELETE FROM group_members WHERE user_id = :uid");
  query.bindValue(":uid", userId);
  query.exec();

  // Dann: Neu zuweisen
  query.prepare(
      "INSERT INTO group_members (group_id, user_id) VALUES (:gid, :uid)");
  query.bindValue(":gid", groupId);
  query.bindValue(":uid", userId);

  return query.exec();
}
