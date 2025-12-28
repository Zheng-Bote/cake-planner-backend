#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp"

class AuthController {
public:
  // Wir übergeben die App als Template oder Referenz
  // Da wir Middleware nutzen, müssen wir den App-Typ kennen
  static void registerRoutes(crow::App<AuthMiddleware> &app);
};