/**
 * @file auth_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Auth Controller Implementation
 * @version 0.4.5
 * @date 2026-01-14
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/auth_controller.hpp"
#include "models/user_model.hpp"
#include "services/notification_service.hpp"
#include "utils/password_utils.hpp"
#include "utils/token_utils.hpp"
#include "utils/totp_utils.hpp"
#include <QDebug>

namespace rz {
namespace controller {

AuthController::AuthController(service::NotificationService* notifyService)
    : m_notifyService(notifyService) {}

// KORRIGIERT: Namespace rz::middleware::AuthMiddleware
void AuthController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app) {

  // 1. REGISTRIERUNG
  CROW_ROUTE(app, "/api/auth/register")
      .methods(crow::HTTPMethod::POST)([this](const crow::request &req) {
        auto json = crow::json::load(req.body);
        if (!json || !json.has("email") || !json.has("password") ||
            !json.has("name")) {
          return crow::response(400, "Missing parameters");
        }

        User user;
        user.email = QString::fromStdString(json["email"].s());
        // Voll qualifizierter Aufruf für utils
        user.password_hash = rz::utils::PasswordUtils::hashPassword(
            QString::fromStdString(json["password"].s()));
        user.full_name = QString::fromStdString(json["name"].s());

        if(json.has("language")) {
            user.language = QString::fromStdString(json["language"].s());
        } else {
            user.language = "en";
        }

        if(json.has("languageEmail")) {
            user.emailLanguage = QString::fromStdString(json["languageEmail"].s());
        } else {
            user.emailLanguage = user.language;
        }

        if (user.create()) {
            // Notification auslösen
            if (m_notifyService) {
                m_notifyService->notifyAdminsNewUser(user.full_name, user.email);
            } else {
                qWarning() << "NotificationService not available inside AuthController!";
            }
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

        if (!rz::utils::PasswordUtils::verifyPassword(password, user.password_hash)) {
          return crow::response(401, "Invalid credentials");
        }

        // 2FA Check
        if (!user.totp_secret.isEmpty()) {
          if (totpCode.isEmpty()) {
            crow::json::wvalue res;
            res["require2fa"] = true;
            return crow::response(200, res);
          }
          if (!rz::utils::TotpUtils::validateCode(user.totp_secret, totpCode)) {
            return crow::response(401, "Invalid 2FA code");
          }
        }

        if (!user.is_active)
          return crow::response(403, "Account inactive");

        auto token =
            rz::utils::TokenUtils::generateToken(user.id, user.email, user.is_admin);
        crow::json::wvalue res;
        res["token"] = token.toStdString();
        res["user"] = user.toJson();
        return crow::response(200, res);
      });

  // 3. 2FA SETUP
  CROW_ROUTE(app, "/api/auth/2fa/setup")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        QString secret = rz::utils::TotpUtils::generateSecret();
        QString otpAuthUrl =
            rz::utils::TotpUtils::getProvisioningUri(ctx.currentUser.email, secret);

        crow::json::wvalue res;
        res["secret"] = secret.toStdString();
        res["otpauth"] = otpAuthUrl.toStdString();

        return crow::response(res);
      });

  // 4. 2FA AKTIVIEREN
  CROW_ROUTE(app, "/api/auth/2fa/activate")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto json = crow::json::load(req.body);
        crow::json::wvalue res;

        if (!json || !json.has("secret") || !json.has("code")) {

            res["error"] = "Missing secret or code";
          return crow::response(400, res);
        }

        QString secret = QString::fromStdString(json["secret"].s());
        QString code = QString::fromStdString(json["code"].s());

        if (rz::utils::TotpUtils::validateCode(secret, code)) {
          auto userOpt = User::getById(ctx.currentUser.userId);
          if (userOpt) {
            User u = *userOpt;
            if (u.enable2FA(secret)) {
                res["message"] = "2FA enabled successfully";
              return crow::response(200, res);
            }
          }
          res["message"] = "Database error";
          return crow::response(500, res);
        } else {
            res["error"] = "Invalid code";
          return crow::response(400, res);
        }
      });
}

} // namespace controller
} // namespace rz
