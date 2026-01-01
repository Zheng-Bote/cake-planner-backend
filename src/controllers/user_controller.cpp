/**
 * @file user_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.4.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/user_controller.hpp"
#include "models/user_model.hpp"
#include "utils/password_utils.hpp" // Für Argon2
#include "utils/token_utils.hpp"    // Falls wir später Tokens brauchen

void UserController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // --- GET /api/users ---
  // Dies ist eine geschützte Route (dank AuthMiddleware in main.cpp)
  // Nur eingeloggte User kommen hierhin.
  CROW_ROUTE(app, "/api/users")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    // 1. Identität prüfen
    if (ctx.currentUser.userId.isEmpty())
      return crow::response(401);

    std::vector<User> users;

    // FALL A: Global Admin -> Sieht ALLES
    if (ctx.currentUser.isAdmin) {
      users = User::getAll(); // Kein Filter
    }
    // FALL B: Prüfen auf Local Admin
    else {
      // Wir holen uns Gruppe und Rolle des Anfragenden
      auto info = User::getGroupAndRole(ctx.currentUser.userId);
      QString myGroupId = info.first;
      QString myRole = info.second;

      if (myRole == "admin" && !myGroupId.isEmpty()) {
        // Local Admin -> Sieht nur SEINE Gruppe
        users = User::getAll(myGroupId);
      } else {
        // FALL C: Normales Mitglied -> Darf Liste nicht sehen
        return crow::response(403, "Forbidden: Insufficient rights.");
      }
    }

    crow::json::wvalue result = crow::json::wvalue::list();
    int i = 0;
    for (const auto &user : users) {
      result[i++] = user.toJson();
    }
    return crow::response(result);
  });

  // --- POST /api/register ---
  // Dies ist öffentlich (in AuthMiddleware Whitelist eingetragen)
  CROW_ROUTE(app, "/api/register")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto json = crow::json::load(req.body);

        // 1. Validierung
        if (!json) {
          return crow::response(400, "Invalid JSON");
        }
        if (!json.has("email") || !json.has("password") || !json.has("name")) {
          return crow::response(400, "Missing fields (email, password, name)");
        }

        QString email = QString::fromStdString(json["email"].s());
        QString plainPassword = QString::fromStdString(json["password"].s());
        QString name = QString::fromStdString(json["name"].s());

        // 2. Dubletten-Check
        if (User::getByEmail(email).has_value()) {
          return crow::response(409, "User already exists");
        }

        // 3. User Objekt vorbereiten
        User newUser;
        newUser.full_name = name;
        newUser.email = email;

        // 4. Argon2 Hashing
        newUser.password_hash = PasswordUtils::hashPassword(plainPassword);

        if (newUser.password_hash.isEmpty()) {
          return crow::response(500, "Internal Error: Hashing failed");
        }

        // Standardmäßig inaktiv, außer es ist der allererste User (Seeding
        // übernimmt das aber meist)
        newUser.is_active = false;
        newUser.is_admin = false;

        // 5. In DB speichern
        if (newUser.create()) {
          crow::json::wvalue resJson;
          resJson["message"] =
              "Registration successful. Please wait for admin activation.";
          resJson["userId"] = newUser.id.toStdString();
          return crow::response(201, resJson);
        } else {
          return crow::response(500, "Database error during registration");
        }
      });

  // --- POST /api/user/change-password ---
  CROW_ROUTE(app, "/api/user/change-password")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        // 1. Auth Check (Middleware nutzen)
        const auto &ctx = app.get_context<AuthMiddleware>(req);
        if (ctx.currentUser.userId.isEmpty()) {
          return crow::response(401, "Unauthorized");
        }

        // 2. Input parsen
        auto json = crow::json::load(req.body);
        if (!json || !json.has("newPassword")) {
          return crow::response(400, "Missing parameter: newPassword");
        }

        std::string newPassRaw = json["newPassword"].s();

        // 3. Validierung (Minimalanforderung)
        if (newPassRaw.length() < 8) {
          return crow::response(400, "Password must be at least 8 chars");
        }

        // 4. Hashing (PasswordUtils nutzen)
        QString newHash =
            PasswordUtils::hashPassword(QString::fromStdString(newPassRaw));
        if (newHash.isEmpty()) {
          return crow::response(500, "Hashing failed");
        }

        // 5. Model aufrufen (MVC)
        if (User::updatePassword(ctx.currentUser.userId, newHash)) {
          return crow::response(200, "Password changed successfully");
        } else {
          return crow::response(500, "Database error");
        }
      });
}
