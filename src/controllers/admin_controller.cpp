/**
 * @file admin_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Admin Controller Implementation with Group Management
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/admin_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include "models/user_model.hpp"
#include "services/notification_service.hpp"
#include "database.hpp"

namespace rz::controller {

/**
 * @brief Registers all admin-related routes with the Crow application.
 *
 * This method sets up the endpoints for user management, group management, and other administrative tasks.
 * It enforces authentication and authorization checks (admin privileges) for sensitive operations.
 *
 * @param app The Crow application instance to register routes with.
 * @param notifyService Pointer to the notification service for sending emails (can be nullptr).
 */
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
      if (info.second == "admin" && !info.first.isEmpty()) {
        users = User::getAll(info.first);
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

  // --- PUT /api/admin/users/<id>/status ---
  CROW_ROUTE(app, "/api/admin/users/<string>/status")
      .methods(crow::HTTPMethod::PUT)([&, notifyService](const crow::request &req, std::string userId) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("isActive")) return crow::response(400);

        bool newActiveStatus = json["isActive"].b();
        QString qUserId = QString::fromStdString(userId);
        auto user = User::getById(qUserId);
        if (!user) return crow::response(404);

        if (User::updateStatus(qUserId, newActiveStatus)) {
            if (notifyService && user->is_active != newActiveStatus) {
                if (newActiveStatus) notifyService->notifyAccountActivated(user->email, user->full_name, user->emailLanguage);
                else notifyService->notifyAccountDeactivated(user->email, user->full_name, user->emailLanguage);
            }
            return crow::response(200, crow::json::wvalue({{"message", "Status updated"}}));
        }
        return crow::response(500);
      });

  // --- DELETE /api/admin/users/<id> ---
  CROW_ROUTE(app, "/api/admin/users/<string>")
      .methods(crow::HTTPMethod::DELETE)([&, notifyService](const crow::request &req, std::string userId) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        QString qUserId = QString::fromStdString(userId);
        auto user = User::getById(qUserId);
        if (!user) return crow::response(404);

        if (User::softDelete(qUserId)) {
            if (notifyService) notifyService->notifyAccountDeleted(user->email, user->full_name, user->emailLanguage);
            return crow::response(200);
        }
        return crow::response(500);
      });

  // --- POST /api/admin/groups (ADMIN ONLY: Create Group) ---
  CROW_ROUTE(app, "/api/admin/groups")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("name")) return crow::response(400);

        QString groupName = QString::fromStdString(json["name"].s());
        QString newGroupId = User::createGroup(groupName);

        if (!newGroupId.isEmpty()) {
            crow::json::wvalue res;
            res["id"] = newGroupId.toStdString();
            res["name"] = groupName.toStdString();
            return crow::response(201, res);
        }
        return crow::response(500, "Group creation failed");
      });

  // --- DELETE /api/admin/groups/<id> (ADMIN ONLY: Delete Group) ---
  CROW_ROUTE(app, "/api/admin/groups/<string>")
      .methods(crow::HTTPMethod::DELETE)([&](const crow::request &req, std::string groupId) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        if (User::deleteGroup(QString::fromStdString(groupId))) {
            return crow::response(200);
        }
        return crow::response(404, "Group not found or not empty");
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
      if (info.second == "admin" && !info.first.isEmpty()) {
        for (const auto &g : allGroups) {
          if (g.first == info.first) {
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

        if (User::assignToGroup(QString::fromStdString(json["userId"].s()), QString::fromStdString(json["groupId"].s()))) {
          return crow::response(200, crow::json::wvalue({{"message", "Group assigned"}}));
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

        if (User::setGroupRole(QString::fromStdString(json["userId"].s()),
                              QString::fromStdString(json["groupId"].s()),
                              QString::fromStdString(json["role"].s()))) {
          return crow::response(200, crow::json::wvalue({{"message", "Role updated"}}));
        }
        return crow::response(500);
      });

      // --- POST /api/admin/users/force-password-change ---
  // Matches Frontend Payload: { "userId": "...", "mustChange": true }
  CROW_ROUTE(app, "/api/admin/users/force-password-change")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        // 1. Security check: Only admins
        if (!ctx.currentUser.isAdmin) return crow::response(403);

        // 2. Parse JSON
        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("mustChange")) {
            return crow::response(400, "Missing userId or mustChange in payload");
        }

        // 3. Extract values
        QString targetUserId = QString::fromStdString(json["userId"].s());
        bool mustChange = json["mustChange"].b();

        // 4. Perform update
        if (User::setMustChangePassword(targetUserId, mustChange)) {
            crow::json::wvalue res;
            res["message"] = "Force password change flag updated";
            res["mustChange"] = mustChange;
            return crow::response(200, res);
        }

        return crow::response(500, "Database error");
      });
}

} // namespace rz::controller
