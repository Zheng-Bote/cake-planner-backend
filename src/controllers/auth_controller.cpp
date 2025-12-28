#include "controllers/auth_controller.hpp"
#include "models/user_model.hpp"
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"

void AuthController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // POST /api/login
  CROW_ROUTE(app, "/api/login")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password")) {
          return crow::response(400, "Missing credentials");
        }

        QString email = QString::fromStdString(json["email"].s());
        QString password = QString::fromStdString(json["password"].s());

        // 1. User suchen
        auto userOpt = User::getByEmail(email);
        if (!userOpt) {
          // Aus Sicherheitsgründen generische Fehlermeldung (User Enumeration
          // verhindern)
          return crow::response(401, "Invalid email or password");
        }

        const User &user = *userOpt;

        // 2. Ist User aktiv?
        if (!user.is_active) {
          return crow::response(403, "Account is not active. Contact admin.");
        }

        // 3. Passwort prüfen (Argon2)
        if (!PasswordUtils::verifyPassword(password, user.password_hash)) {
          return crow::response(401, "Invalid email or password");
        }

        // 4. Token ausstellen
        QString token =
            TokenUtils::generateToken(user.id, user.email, user.is_admin);

        crow::json::wvalue result;
        result["token"] = token.toStdString();
        result["user"]["id"] = user.id.toStdString();
        result["user"]["name"] = user.full_name.toStdString();
        result["user"]["isAdmin"] = user.is_admin;

        return crow::response(200, result);
      });
}