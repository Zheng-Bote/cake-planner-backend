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
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"

namespace rz {
namespace controller {

// WICHTIG: Signatur muss rz::middleware::AuthMiddleware enthalten
void UserController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app) {

  // --- GET /api/users ---
  CROW_ROUTE(app, "/api/users")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

    if (ctx.currentUser.userId.isEmpty())
      return crow::response(401);

    std::vector<User> users;

    if (ctx.currentUser.isAdmin) {
      users = User::getAll();
    } else {
      auto info = User::getGroupAndRole(ctx.currentUser.userId);
      QString myGroupId = info.first;
      QString myRole = info.second;

      if (myRole == "admin" && !myGroupId.isEmpty()) {
        users = User::getAll(myGroupId);
      } else {
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
  CROW_ROUTE(app, "/api/register")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password") || !json.has("name")) {
          return crow::response(400, "Missing fields");
        }

        QString email = QString::fromStdString(json["email"].s());
        QString plainPassword = QString::fromStdString(json["password"].s());
        QString name = QString::fromStdString(json["name"].s());

        if (User::getByEmail(email).has_value()) {
          return crow::response(409, "User already exists");
        }

        User newUser;
        newUser.full_name = name;
        newUser.email = email;

        // Namespace rz::utils nutzen!
        newUser.password_hash = rz::utils::PasswordUtils::hashPassword(plainPassword);

        if (newUser.password_hash.isEmpty()) return crow::response(500, "Hashing failed");

        newUser.is_active = false;
        newUser.is_admin = false;

        if (newUser.create()) {
          crow::json::wvalue resJson;
          resJson["message"] = "Registration successful.";
          resJson["userId"] = newUser.id.toStdString();
          return crow::response(201, resJson);
        } else {
          return crow::response(500, "Database error");
        }
      });

  // --- POST /api/user/change-password ---
  CROW_ROUTE(app, "/api/user/change-password")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (ctx.currentUser.userId.isEmpty()) return crow::response(401);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("newPassword")) return crow::response(400);

        std::string newPassRaw = json["newPassword"].s();
        if (newPassRaw.length() < 8) return crow::response(400, "Min 8 chars");

        QString newHash = rz::utils::PasswordUtils::hashPassword(QString::fromStdString(newPassRaw));
        if (newHash.isEmpty()) return crow::response(500);

        if (User::updatePassword(ctx.currentUser.userId, newHash)) {
          return crow::response(200, "Password changed");
        }
        return crow::response(500, "DB Error");
      });

    // Profil-Update (Sprache)
    CROW_ROUTE(app, "/api/user/settings")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto json = crow::json::load(req.body);
        if (!json) return crow::response(400);

        std::string lang = json["language"].s();
        if (User::updateSettings(ctx.currentUser.userId, QString::fromStdString(lang))) {
            return crow::response(200);
        }
        return crow::response(500);
    });

    // Account Löschen
    CROW_ROUTE(app, "/api/user")
    .methods(crow::HTTPMethod::DELETE)
    ([&](const crow::request& req){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (User::softDelete(ctx.currentUser.userId)) {
            return crow::response(200, "Account deleted");
        }
        return crow::response(500);
    });
}

} // namespace controller
} // namespace rz
