#pragma once
#include "crow.h"
#include "utils/token_utils.hpp"

struct AuthMiddleware {
  // Kontext speichert Daten für den Controller (z.B. "Wer ist eingeloggt?")
  struct context {
    TokenPayload currentUser;
  };

  void before_handle(crow::request &req, crow::response &res, context &ctx) {
    // 1. Whitelist: Öffentliche Routen ignorieren
    std::string url = req.url;
    if (url == "/api/login" || url == "/api/register" || url == "/api/status" ||
        url.starts_with("/static")) {
      return;
    }

    // 2. Header prüfen
    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.empty() || !authHeader.starts_with("Bearer ")) {
      res.code = 401;
      res.body = "Unauthorized: Missing or invalid token format.";
      res.end();
      return;
    }

    // 3. Token extrahieren ( "Bearer " abschneiden )
    std::string token = authHeader.substr(7);

    // 4. Verifizieren
    auto payload = TokenUtils::verifyToken(token);
    if (!payload) {
      res.code = 403; // Forbidden (Token ungültig/abgelaufen)
      res.body = "Forbidden: Invalid or expired token.";
      res.end();
      return;
    }

    // 5. User-Daten im Kontext speichern (für Controller verfügbar)
    ctx.currentUser = *payload;
  }

  void after_handle(crow::request &req, crow::response &res, context &ctx) {
    // Nichts zu tun nach dem Request
  }
};