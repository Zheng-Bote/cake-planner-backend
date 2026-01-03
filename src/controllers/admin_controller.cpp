/**
 * @file admin_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.4.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */


#include "controllers/admin_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include "models/user_model.hpp"
#include "database.hpp" // Global namespace

namespace rz {
namespace controller {

void AdminController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app) {

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

  // --- POST /api/admin/users/toggle-active ---
  CROW_ROUTE(app, "/api/admin/users/toggle-active")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("isActive")) return crow::response(400);

        QString userId = QString::fromStdString(json["userId"].s());
        bool isActive = json["isActive"].b();

        if (User::updateStatus(userId, isActive)) {
          crow::json::wvalue res; res["message"] = "Status updated";
          return crow::response(200, res);
        }
        return crow::response(500);
      });

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
