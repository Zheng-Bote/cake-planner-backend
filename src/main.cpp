/**
 * @file main.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Entry Point
 * @version 0.3.5
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "crow.h"
#include "database.hpp"
#include "rz_config.hpp"
#include "utils/env_loader.hpp"
#include "utils/seeder.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <thread> // Wichtig für Server-Thread

// Middleware & Controller Includes
#include "controllers/admin_controller.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/event_controller.hpp"
#include "controllers/user_controller.hpp"
#include "middleware/auth_middleware.hpp"

// SMTP & Models
#include "models/config_model.hpp" // Achte auf Groß/Kleinschreibung im Dateinamen!
#include "services/smtp_service.hpp"
#include "services/notification_service.hpp"

int main(int argc, char *argv[]) {
  // 1. Qt Core Application (Startet die Event-Loop für SMTP)
  QCoreApplication qtApp(argc, argv);

  qInfo() << "Starte" << rz::config::PROG_LONGNAME.data() << "v"
          << rz::config::VERSION.data();

  // 2. Environment laden
  rz::utils::EnvLoader::load("CakePlanner.env");
  int serverPort = rz::utils::EnvLoader::getInt("CAKE_SERVER_PORT", 8080);

  // 3. Datenbank
  DatabaseManager::instance().initialize("data/cakeplanner.sqlite");
  if (!DatabaseManager::instance().migrate()) {
    qCritical() << "Abbruch: Datenbank-Migration fehlgeschlagen.";
    return -1;
  }
  rz::utils::Seeder::ensureAdminExists();

  // 4. Services Setup (Dependency Injection)
  rz::model::ConfigModel configModel;
  configModel.loadEnv("CakePlanner.env");

  rz::service::SmtpService smtpService(configModel, &qtApp);
  rz::service::NotificationService notifyService(&smtpService);

  // 5. Crow App mit Middleware (Namespace beachten!)
  crow::App<rz::middleware::AuthMiddleware> app;

  // 6. Controller Registrierung

  // A) AuthController muss instanziiert werden (für NotificationService)
  rz::controller::AuthController authController(&notifyService);
  authController.registerRoutes(app);

  // B) Andere Controller (statisch) - Namespace explizit angeben
rz::controller::UserController::registerRoutes(app);
rz::controller::EventController::registerRoutes(app, &notifyService);
  rz::controller::AdminController::registerRoutes(app);

  // Test-Route
  CROW_ROUTE(app, "/api/profile")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
    crow::json::wvalue result;
    result["msg"] = "Hello authenticated user!";
    result["your_email"] = ctx.currentUser.email.toStdString();
    return result;
  });

  qInfo() << "Server lauscht auf Port:" << serverPort;

  // Crow in eigenem Thread starten, damit Qt-Loop (für Mail) weiterläuft
  std::thread serverThread([&app, serverPort](){
    // Erhöhe Threads auf z.B. 20
    // app.port(serverPort).concurrency(20).run();
    app.port(serverPort).multithreaded().run();
  });

  // Qt Event Loop starten
  return qtApp.exec();
}
