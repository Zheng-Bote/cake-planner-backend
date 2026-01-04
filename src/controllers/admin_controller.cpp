/**
 * @file admin_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Admin Controller Implementation with Notifications
 * @version 0.3.1
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/admin_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include "models/user_model.hpp"
#include "services/notification_service.hpp" // WICHTIG: Für E-Mails
#include "database.hpp"

namespace rz {
namespace controller {

// Signatur angepasst: Nimmt notifyService entgegen
void AdminController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) {

  // --- GET /api/admin/users ---
  CROW_ROUTE(app, "/api/admin/users")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
    if (ctx.currentUser.userId.isEmpty()) return crow::response(401);

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
        return crow::response(403);
      }
    }

    crow::json::wvalue result = crow::json::wvalue::list();
    for (size_t i = 0; i < users.size(); ++i) {
      result[i] = users[i].toJson();
    }
    return crow::response(result);
  });

  // --- PUT /api/admin/users/<id>/status (Status ändern & E-Mail senden) ---
  // Ersetzt das alte "toggle-active"
  CROW_ROUTE(app, "/api/admin/users/<string>/status")
      .methods(crow::HTTPMethod::PUT)([&, notifyService](const crow::request &req, std::string userId) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("isActive")) return crow::response(400);

        bool newActiveStatus = json["isActive"].b();
        QString qUserId = QString::fromStdString(userId);

        // 1. User laden um alten Status und E-Mail zu bekommen
        auto user = User::getById(qUserId);
        if (!user) return crow::response(404, "User not found");

        bool oldStatus = user->is_active;

        // 2. Update durchführen
        if (User::updateStatus(qUserId, newActiveStatus)) {

            // 3. E-Mail senden bei Statusänderung
            if (notifyService && oldStatus != newActiveStatus) {
                if (newActiveStatus) {
                    // Aktiviert
                    notifyService->notifyAccountActivated(user->email, user->full_name, user->emailLanguage);
                } else {
                    // Deaktiviert
                    notifyService->notifyAccountDeactivated(user->email, user->full_name, user->emailLanguage);
                }
            }

            crow::json::wvalue res;
            res["message"] = "Status updated";
            return crow::response(200, res);
        }
        return crow::response(500);
      });

  // --- DELETE /api/admin/users/<id> (Löschen & E-Mail senden) ---
  // NEU HINZUGEFÜGT
  CROW_ROUTE(app, "/api/admin/users/<string>")
      .methods(crow::HTTPMethod::DELETE)([&, notifyService](const crow::request &req, std::string userId) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        QString qUserId = QString::fromStdString(userId);

        // 1. User vorher laden (Daten sichern)
        auto user = User::getById(qUserId);
        if (!user) return crow::response(404);

        QString email = user->email;
        QString name = user->full_name;
        QString lang = user->emailLanguage;

        // 2. Soft Delete ausführen
        if (User::softDelete(qUserId)) {

            // 3. E-Mail senden
            if (notifyService) {
                notifyService->notifyAccountDeleted(email, name, lang);
            }
            return crow::response(200);
        }
        return crow::response(500);
      });

  // ==========================================
  // LEGACY ROUTES (Fix für 405 Error im Admin-Panel)
  // ==========================================

  // --- POST /api/admin/users/toggle-active ---
  CROW_ROUTE(app, "/api/admin/users/toggle-active")
      .methods(crow::HTTPMethod::POST)([&, notifyService](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("isActive")) return crow::response(400);

        QString userId = QString::fromStdString(json["userId"].s());
        bool newActiveStatus = json["isActive"].b();

        // 1. User laden (für E-Mail Check)
        auto user = User::getById(userId);
        if (!user) return crow::response(404, "User not found");

        bool oldStatus = user->is_active;

        // 2. Status Update
        if (User::updateStatus(userId, newActiveStatus)) {
            // 3. E-Mail Trigger
            if (notifyService && oldStatus != newActiveStatus) {
                if (newActiveStatus) {
                    notifyService->notifyAccountActivated(user->email, user->full_name, user->emailLanguage);
                } else {
                    notifyService->notifyAccountDeactivated(user->email, user->full_name, user->emailLanguage);
                }
            }
            crow::json::wvalue res; res["message"] = "Status updated";
            return crow::response(200, res);
        }
        return crow::response(500);
      });

  // ==========================================
  // LEGACY ROUTES (Fix für 405 Error im Admin-Panel)
  // ==========================================

  // --- POST /api/admin/users/force-password-change ---
  CROW_ROUTE(app, "/api/admin/users/force-password-change")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("mustChange")) return crow::response(400);

        QString userId = QString::fromStdString(json["userId"].s());
        bool mustChange = json["mustChange"].b();

        if (User::setMustChangePassword(userId, mustChange)) {
          crow::json::wvalue res; res["message"] = "Flag updated";
          return crow::response(200, res);
        }
        return crow::response(500);
      });

  // --- POST /api/admin/groups/set-role ---
  CROW_ROUTE(app, "/api/admin/groups/set-role")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json) return crow::response(400);

        QString uid = QString::fromStdString(json["userId"].s());
        QString gid = QString::fromStdString(json["groupId"].s());
        QString role = QString::fromStdString(json["role"].s());

        if (User::setGroupRole(uid, gid, role)) {
          crow::json::wvalue res; res["message"] = "Role updated";
          return crow::response(200, res);
        }
        return crow::response(500);
      });

  // --- GET /api/admin/groups ---
  CROW_ROUTE(app, "/api/admin/groups")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
    if (ctx.currentUser.userId.isEmpty()) return crow::response(401);

    auto allGroups = User::getAllGroups();
    crow::json::wvalue json = crow::json::wvalue::list();
    int idx = 0;

    if (ctx.currentUser.isAdmin) {
      for (const auto &g : allGroups) {
        json[idx]["id"] = g.first.toStdString();
        json[idx]["name"] = g.second.toStdString();
        idx++;
      }
    } else {
      auto info = User::getGroupAndRole(ctx.currentUser.userId);
      QString myGroupId = info.first;
      QString myRole = info.second;

      if (myRole == "admin" && !myGroupId.isEmpty()) {
        for (const auto &g : allGroups) {
          if (g.first == myGroupId) {
            json[idx]["id"] = g.first.toStdString();
            json[idx]["name"] = g.second.toStdString();
            idx++;
          }
        }
      } else {
        return crow::response(403);
      }
    }
    return crow::response(json);
  });

  // --- POST /api/admin/users/assign-group ---
  CROW_ROUTE(app, "/api/admin/users/assign-group")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("groupId")) return crow::response(400);

        QString uid = QString::fromStdString(json["userId"].s());
        QString gid = QString::fromStdString(json["groupId"].s());

        if (User::assignToGroup(uid, gid)) {
          crow::json::wvalue res; res["message"] = "Group assigned";
          return crow::response(200, res);
        }
        return crow::response(500);
      });
}

} // namespace controller
} // namespace rz
