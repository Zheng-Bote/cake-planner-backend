#pragma once
#include "crow.h"
#include "middleware/auth_middleware.hpp"

class EventController {
public:
  static void registerRoutes(crow::App<AuthMiddleware> &app);
};