#include "crow.h"
#include "database.hpp"
#include "rz_config.hpp"
#include "utils/env_loader.hpp"
#include "utils/seeder.hpp"

#include <QCoreApplication>
#include <QDebug>

// Middleware & Controller Includes
#include "controllers/auth_controller.hpp"
#include "controllers/event_controller.hpp"
#include "controllers/user_controller.hpp"
#include "middleware/auth_middleware.hpp"

int main(int argc, char *argv[]) {
  QCoreApplication qtApp(argc, argv);

  // 1. App Info aus rz_config setzen
  qInfo() << "Starte" << rz::config::PROG_LONGNAME.data() << "v"
          << rz::config::VERSION.data();

  // 2. Environment laden (CakePlanner.env)
  EnvLoader::load("CakePlanner.env");

  // Konfigurationswerte abrufen
  int serverPort = EnvLoader::getInt("CAKE_SERVER_PORT", 8080);
  QString adminPwd = EnvLoader::get("CAKE_ADMIN_PASSWORD", "admin123");
  // TODO: adminPwd später an den DatabaseManager oder AuthMiddleware übergeben

  // 3. Datenbank Initialisieren
  DatabaseManager::instance().initialize("data/cakeplanner.sqlite");

  // Auto-Migration beim Start
  if (!DatabaseManager::instance().migrate()) {
    qCritical() << "Abbruch: Datenbank-Migration fehlgeschlagen.";
    return -1;
  }

  // Stellt sicher, dass wir uns einloggen können
  Seeder::ensureAdminExists();

  // --- CROW APP MIT MIDDLEWARE ---
  crow::App<AuthMiddleware> app;

  // Routen registrieren
  // Achtung: UserController muss angepasst werden, um crow::App<AuthMiddleware>
  // zu akzeptieren, oder wir machen registerRoutes zum Template.
  AuthController::registerRoutes(app);
  UserController::registerRoutes(app);
  EventController::registerRoutes(app);

  // Beispiel für geschützte Route (liest User aus Kontext):
  CROW_ROUTE(app, "/api/profile")
  ([&](const crow::request &req) {
    // Zugriff auf Middleware-Kontext
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    crow::json::wvalue result;
    result["msg"] = "Hello authenticated user!";
    result["your_email"] = ctx.currentUser.email.toStdString();
    return result;
  });

  // 5. Server starten
  qInfo() << "Server lauscht auf Port:" << serverPort;
  app.port(serverPort).multithreaded().run();

  return 0;
}