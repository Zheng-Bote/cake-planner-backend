#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp" // WICHTIG: Middleware einbinden

class AdminController {
public:
  // Wir akzeptieren jetzt spezifisch die App MIT AuthMiddleware
  static void registerRoutes(crow::App<AuthMiddleware> &app);
};