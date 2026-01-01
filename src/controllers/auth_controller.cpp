/**
 * @file auth_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/auth_controller.hpp"
#include "models/user_model.hpp"
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"
#include "utils/totp_utils.hpp"
#include <QDebug>

void AuthController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // 1. REGISTRIERUNG
  CROW_ROUTE(app, "/api/auth/register")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password") ||
            !json.has("name")) {
          return crow::response(400, "Missing parameters");
        }

        User user;
        user.email = QString::fromStdString(json["email"].s());
        user.password_hash = PasswordUtils::hashPassword(
            QString::fromStdString(json["password"].s()));
        user.full_name = QString::fromStdString(json["name"].s());

        // Erster User wird Admin (Logik aus User Model)
        if (user.create()) {
          return crow::response(201, "User created");
        } else {
          return crow::response(400, "User already exists or database error");
        }
      });

  // 2. LOGIN
  CROW_ROUTE(app, "/api/login")
      .methods(crow::HTTPMethod::POST)([](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password")) {
          return crow::response(400, "Missing credentials");
        }

        QString email = QString::fromStdString(json["email"].s());
        QString password = QString::fromStdString(json["password"].s());
        QString totpCode;
        if (json.has("code")) {
          totpCode = QString::fromStdString(json["code"].s());
        }

        auto userOpt = User::getByEmail(email);
        if (!userOpt)
          return crow::response(401, "Invalid credentials");
        User user = *userOpt;

        if (!PasswordUtils::verifyPassword(password, user.password_hash)) {
          return crow::response(401, "Invalid credentials");
        }

        // 2FA Check
        if (!user.totp_secret.isEmpty()) {
          if (totpCode.isEmpty()) {
            crow::json::wvalue res;
            res["require2fa"] = true;
            return crow::response(200, res);
          }
          if (!TotpUtils::validateCode(user.totp_secret, totpCode)) {
            return crow::response(401, "Invalid 2FA code");
          }
        }

        if (!user.is_active)
          return crow::response(403, "Account inactive");

        auto token =
            TokenUtils::generateToken(user.id, user.email, user.is_admin);
        crow::json::wvalue res;
        res["token"] = token.toStdString();
        res["user"] = user.toJson();
        return crow::response(200, res);
      });

  // 3. 2FA SETUP (Secret generieren)
  CROW_ROUTE(app, "/api/auth/2fa/setup")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        QString secret = TotpUtils::generateSecret();
        QString otpAuthUrl =
            TotpUtils::getProvisioningUri(ctx.currentUser.email, secret);

        crow::json::wvalue res;
        res["secret"] = secret.toStdString();
        res["otpauth"] = otpAuthUrl.toStdString();

        return crow::response(res);
      });

  // 4. 2FA AKTIVIEREN (Code prüfen und speichern)
  CROW_ROUTE(app, "/api/auth/2fa/activate")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);
        auto json = crow::json::load(req.body);

        if (!json || !json.has("secret") || !json.has("code")) {
          return crow::response(400, "Missing secret or code");
        }

        QString secret = QString::fromStdString(json["secret"].s());
        QString code = QString::fromStdString(json["code"].s());

        if (TotpUtils::validateCode(secret, code)) {
          auto userOpt = User::getById(ctx.currentUser.userId);
          if (userOpt) {
            // Wir müssen enable2FA auf der INSTANZ aufrufen
            User u = *userOpt;
            if (u.enable2FA(secret)) {
              return crow::response(200, "2FA enabled successfully");
            }
          }
          return crow::response(500, "Database error");
        } else {
          return crow::response(400, "Invalid code");
        }
      });
}