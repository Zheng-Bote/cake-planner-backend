/**
 * @file user_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Controller with Email Notifications
 * @version 0.4.5
 * @date 2026-01-14
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/user_controller.hpp"
#include "models/user_model.hpp"
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"
#include "services/notification_service.hpp" // NEU: Include

namespace rz {
namespace controller {

// Signatur Update: notifyService
void UserController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) {

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
      .methods(crow::HTTPMethod::POST)([&, notifyService](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password") || !json.has("name") || !json.has("language") || !json.has("email")) {
          crow::json::wvalue res;
          res["message"] = "Missing fields";
          return crow::response(400, res);
        }

        QString email = QString::fromStdString(json["email"].s());
        QString plainPassword = QString::fromStdString(json["password"].s());
        QString name = QString::fromStdString(json["name"].s());
        QString language = QString::fromStdString(json["language"].s());
        QString languageEmail;

        if(json.has("languageEmail")) {
            languageEmail = QString::fromStdString(json["languageEmail"].s());
        } else {
            languageEmail = language;
        }

        if (User::getByEmail(email).has_value()) {
            crow::json::wvalue res;
            res["message"] = "User already exists";
            return crow::response(409, res);
        }

        User newUser;
        newUser.full_name = name;
        newUser.email = email;
        newUser.language = language;
        newUser.emailLanguage = languageEmail;
        newUser.password_hash = rz::utils::PasswordUtils::hashPassword(plainPassword);

        if (newUser.password_hash.isEmpty()) return crow::response(500, "Hashing failed");

        newUser.is_active = false;
        newUser.is_admin = false;

        crow::json::wvalue resJson;
        if (newUser.create()) {
          resJson["message"] = "Registration successful.";
          resJson["userId"] = newUser.id.toStdString();
            // Notification auslösen
            if (notifyService) {
                notifyService->notifyAdminsNewUser(newUser.full_name, newUser.email);
            } else {
                qWarning() << "NotificationService not available inside UserController!";
            }
            return crow::response(201, resJson);
        } else {
            resJson["message"] = "User already exists or database error";
            return crow::response(400, resJson);
        }

      });

  // --- POST /api/user/change-password ---
  CROW_ROUTE(app, "/api/user/change-password")
      .methods(crow::HTTPMethod::POST)([&, notifyService](const crow::request &req) { // notifyService capturen
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (ctx.currentUser.userId.isEmpty()) return crow::response(401);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("newPassword")) return crow::response(400);

        // 1. User für E-Mail laden
        auto user = User::getById(ctx.currentUser.userId);
        if (!user) return crow::response(404, "User not found");

        std::string newPassRaw = json["newPassword"].s();
        if (newPassRaw.length() < 8) return crow::response(400, "Min 8 chars");

        QString newHash = rz::utils::PasswordUtils::hashPassword(QString::fromStdString(newPassRaw));
        if (newHash.isEmpty()) return crow::response(500);

        if (User::updatePassword(ctx.currentUser.userId, newHash)) {

            // 2. E-Mail senden
            if (notifyService) {
                notifyService->notifyPasswordChanged(user->email, user->full_name, user->emailLanguage);
            }

            crow::json::wvalue res;
            res["message"] = "Password changed";
            return crow::response(200, res);
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

        std::string langEmail;
        std::string lang;
        crow::json::wvalue res;

        if(json.has("languageEmail")) {
            langEmail = json["languageEmail"].s();
            if (User::updateEmailLanguage(ctx.currentUser.userId, QString::fromStdString(langEmail))) {
                res["message"] = "Language email changed";
                return crow::response(200, res);
            }
        }
        if(json.has("language")) {
            lang = json["language"].s();
            if (User::updateLanguage(ctx.currentUser.userId, QString::fromStdString(lang))) {
                res["message"] = "Language changed";
                return crow::response(200, res);
            }
        }
        res["message"] = "Settings not changed";
        return crow::response(500, res);
    });

    // Account Löschen (Self-Delete)
    CROW_ROUTE(app, "/api/user")
    .methods(crow::HTTPMethod::DELETE)
    ([&, notifyService](const crow::request& req){ // notifyService capturen
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        // Daten für E-Mail sichern
        auto user = User::getById(ctx.currentUser.userId);

        if (User::softDelete(ctx.currentUser.userId)) {
            // E-Mail senden (Account Deleted)
            if (notifyService && user) {
                notifyService->notifyAccountDeleted(user->email, user->full_name, user->emailLanguage);
            }
            crow::json::wvalue res;
            res["message"] = "Account deleted";
            return crow::response(200, res);
        }
        return crow::response(500);
    });
}

} // namespace controller
} // namespace rz
