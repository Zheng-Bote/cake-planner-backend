/**
 * @file user_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief User Controller with Email Notifications
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/user_controller.hpp"
#include "models/user_model.hpp"
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"
#include "services/notification_service.hpp" // NEW: Include

#include "spdlog/spdlog.h"

namespace rz {
namespace controller {

// Signature Update: notifyService
/**
 * @brief Registers user-related routes with the Crow application.
 *
 * This method sets up endpoints for user profile management, such as registration, password change,
 * updating settings (language), and account deletion. It also handles listing users for non-admin users.
 *
 * @param app The Crow application instance to register routes with.
 * @param notifyService Pointer to the notification service for email notifications.
 */
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

  // --- GET /api/user/groups ---
  // Returns all groups where the current user is a member
  CROW_ROUTE(app, "/api/user/groups")
  .methods(crow::HTTPMethod::GET)
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

    // 1. Check if user is logged in
    if (ctx.currentUser.userId.isEmpty()) {
        return crow::response(401);
    }

    // 2. Get data from model
    auto memberships = User::getGroupsForUser(ctx.currentUser.userId);

    // 3. Build JSON
    crow::json::wvalue json = crow::json::wvalue::list();
    for (size_t i = 0; i < memberships.size(); ++i) {
        json[i]["id"] = memberships[i].groupId.toStdString();
        json[i]["name"] = memberships[i].groupName.toStdString();
        json[i]["role"] = memberships[i].role.toStdString();
    }

    return crow::response(200, json);
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
            // Trigger notification
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
      .methods(crow::HTTPMethod::POST)([&, notifyService](const crow::request &req) { // capture notifyService
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (ctx.currentUser.userId.isEmpty()) return crow::response(401);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("newPassword")) return crow::response(400);

        // 1. Load user for email
        auto user = User::getById(ctx.currentUser.userId);
        if (!user) return crow::response(404, "User not found");

        std::string newPassRaw = json["newPassword"].s();
        if (newPassRaw.length() < 8) return crow::response(400, "Min 8 chars");

        QString newHash = rz::utils::PasswordUtils::hashPassword(QString::fromStdString(newPassRaw));
        if (newHash.isEmpty()) return crow::response(500);

        if (User::updatePassword(ctx.currentUser.userId, newHash)) {

            spdlog::info("[USER] Password changed for {}", user->email.toStdString());
            // 2. Send email
            if (notifyService) {
                notifyService->notifyPasswordChanged(user->email, user->full_name, user->emailLanguage);
            }

            crow::json::wvalue res;
            res["message"] = "Password changed";
            return crow::response(200, res);
        }
        return crow::response(500, "DB Error");
      });

    // Profile Update (Language)
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

    // Account Deletion (Self-Delete)
    CROW_ROUTE(app, "/api/user")
    .methods(crow::HTTPMethod::DELETE)
    ([&, notifyService](const crow::request& req){ // capture notifyService
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        // Save data for email
        auto user = User::getById(ctx.currentUser.userId);

        if (User::softDelete(ctx.currentUser.userId)) {
            // Send email (Account Deleted)
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
