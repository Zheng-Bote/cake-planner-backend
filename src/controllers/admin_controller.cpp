#include "controllers/admin_controller.hpp"
#include "middleware/auth_middleware.hpp"
#include "models/user_model.hpp"

#include "database.hpp"
#include <QSqlQuery>
// ----------------------------------

void AdminController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // GET /api/admin/users
  CROW_ROUTE(app, "/api/admin/users")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    // 1. Sicherheits-Check: Ist es ein Admin?
    if (!ctx.currentUser.isAdmin) {
      return crow::response(403, "Access denied. Admins only.");
    }

    // 2. Daten laden
    auto users = User::getAll();

    // 3. JSON bauen
    crow::json::wvalue result = crow::json::wvalue::list();
    for (size_t i = 0; i < users.size(); ++i) {
      result[i] = users[i].toJson();
    }

    return crow::response(result);
  });

  // POST /api/admin/users/toggle-active
  CROW_ROUTE(app, "/api/admin/users/toggle-active")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        if (!ctx.currentUser.isAdmin)
          return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("isActive")) {
          return crow::response(400, "Missing params");
        }

        QString userId = QString::fromStdString(json["userId"].s());
        bool isActive = json["isActive"].b();

        // SAUBER: Aufruf der Model-Methode statt rohem SQL
        if (User::updateStatus(userId, isActive)) {
          return crow::response(200, "Updated");
        } else {
          return crow::response(500, "Database error");
        }
      });

  CROW_ROUTE(app, "/api/admin/users/force-password-change")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.currentUser.isAdmin)
          return crow::response(403);

        auto json = crow::json::load(req.body);
        if (!json || !json.has("userId") || !json.has("mustChange"))
          return crow::response(400, "Missing params");

        QString userId = QString::fromStdString(json["userId"].s());
        bool mustChange = json["mustChange"].b();

        if (User::setMustChangePassword(userId, mustChange)) {
          return crow::response(200, "Updated");
        }
        return crow::response(500, "DB Error");
      });
}