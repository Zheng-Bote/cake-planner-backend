/**
 * @file admin_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.2.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/admin_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include "models/user_model.hpp"

#include "database.hpp"
#include <QSqlQuery>
// ----------------------------------

void AdminController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // --- GET /api/admin/users ---
  CROW_ROUTE(app, "/api/admin/users")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    // Identitäts-Check (User muss eingeloggt sein)
    if (ctx.currentUser.userId.isEmpty()) {
      return crow::response(401, "Unauthorized");
    }

    std::vector<User> users;

    // FALL A: Global Admin -> Darf ALLES sehen
    if (ctx.currentUser.isAdmin) {
      users = User::getAll();
    }
    // FALL B: Prüfen auf Local Admin (Gruppen-Admin)
    else {
      // Wir holen uns Gruppe und Rolle des Anfragenden
      auto info = User::getGroupAndRole(ctx.currentUser.userId);
      QString myGroupId = info.first;
      QString myRole = info.second;

      // Wenn er "admin" in einer Gruppe ist -> Zeige User dieser Gruppe
      if (myRole == "admin" && !myGroupId.isEmpty()) {
        users = User::getAll(myGroupId);
      } else {
        // FALL C: Weder Global noch Local Admin -> Zugriff verweigert
        return crow::response(403, "Forbidden: Insufficient rights.");
      }
    }

    // JSON bauen
    crow::json::wvalue result = crow::json::wvalue::list();
    for (size_t i = 0; i < users.size(); ++i) {
      result[i] = users[i].toJson();
    }

    return crow::response(result);
  });

  // --- POST /api/admin/users/toggle-active ---
  CROW_ROUTE(app, "/api/admin/users/toggle-active")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        // TODO: Hier könnte man noch verfeinern, dass Local Admins
        // nur User ihrer eigenen Gruppe deaktivieren dürfen.
        // Aktuell lassen wir nur Global Admins User sperren (sicherer Start).
        if (!ctx.currentUser.isAdmin)
          return crow::response(403, "Global Admin required");

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("isActive")) {
          return crow::response(400, "Missing params");
        }

        QString userId = QString::fromStdString(json["userId"].s());
        bool isActive = json["isActive"].b();

        if (User::updateStatus(userId, isActive)) {
          // FIX: JSON zurückgeben
          crow::json::wvalue res;
          res["message"] = "Status updated";
          return crow::response(200, res);
        } else {
          return crow::response(500, "Database error");
        }
      });

  // --- POST /api/admin/users/force-password-change ---
  CROW_ROUTE(app, "/api/admin/users/force-password-change")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        // Auch hier vorerst nur Global Admin
        if (!ctx.currentUser.isAdmin)
          return crow::response(403, "Global Admin required");

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("mustChange"))
          return crow::response(400, "Missing params");

        QString userId = QString::fromStdString(json["userId"].s());
        bool mustChange = json["mustChange"].b();

        if (User::setMustChangePassword(userId, mustChange)) {
          // FIX: JSON zurückgeben
          crow::json::wvalue res;
          res["message"] = "Password force flag updated";
          return crow::response(200, res);
        }
        return crow::response(500, "DB Error");
      });

  // --- POST /api/admin/groups/set-role ---
  // (Diesen Endpunkt hattest du evtl. in user_controller oder hier,
  // ich füge ihn der Vollständigkeit halber sauber hier ein, falls er fehlte)
  CROW_ROUTE(app, "/api/admin/groups/set-role")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) {
          return crow::response(403, "Forbidden");
        }

        auto json = crow::json::load(req.body);
        if (!json)
          return crow::response(400, "Invalid JSON");

        QString uid = QString::fromStdString(json["userId"].s());
        QString gid = QString::fromStdString(json["groupId"].s());
        QString role = QString::fromStdString(json["role"].s());

        if (User::setGroupRole(uid, gid, role)) {
          // FIX: JSON zurückgeben
          crow::json::wvalue res;
          res["message"] = "Role updated successfully";
          return crow::response(200, res);
        } else {
          return crow::response(500, "Database error");
        }
      });

  // --- GET /api/admin/groups ---
  CROW_ROUTE(app, "/api/admin/groups")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    if (ctx.currentUser.userId.isEmpty())
      return crow::response(401);

    // Alle Gruppen aus der DB holen
    auto allGroups = User::getAllGroups();

    crow::json::wvalue json = crow::json::wvalue::list();
    int idx = 0;

    // FALL A: Global Admin -> Sieht ALLE Gruppen
    if (ctx.currentUser.isAdmin) {
      for (const auto &g : allGroups) {
        json[idx]["id"] = g.first.toStdString();
        json[idx]["name"] = g.second.toStdString();
        idx++;
      }
    }
    // FALL B: Local Admin -> Sieht nur SEINE Gruppe
    else {
      auto info = User::getGroupAndRole(ctx.currentUser.userId);
      QString myGroupId = info.first;
      QString myRole = info.second;

      if (myRole == "admin" && !myGroupId.isEmpty()) {
        // Wir filtern die Liste und geben nur die eigene Gruppe zurück
        for (const auto &g : allGroups) {
          if (g.first == myGroupId) {
            json[idx]["id"] = g.first.toStdString();
            json[idx]["name"] = g.second.toStdString();
            idx++;
          }
        }
      } else {
        return crow::response(403, "Forbidden");
      }
    }

    return crow::response(json);
  });

  // POST /api/admin/users/assign-group
  CROW_ROUTE(app, "/api/admin/users/assign-group")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin)
          return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("groupId"))
          return crow::response(400);

        QString uid = QString::fromStdString(json["userId"].s());
        QString gid = QString::fromStdString(json["groupId"].s());

        if (User::assignToGroup(uid, gid)) {
          // FIX: JSON Body für das Frontend mitsenden
          crow::json::wvalue res;
          res["message"] = "Group assigned successfully";
          return crow::response(200, res);
        }
        return crow::response(500);
      });
}
