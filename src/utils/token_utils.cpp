/**
 * @file token_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief JWT Token Utilities Implementation
 * @version 0.1.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils/token_utils.hpp"
#include "utils/env_loader.hpp"
#include <chrono>
#include <iostream>

// Zugriff auf JSON Traits für Bool-Konvertierung
using json_value = jwt::traits::kazuho_picojson::value_type;

// KORREKTUR: Alles in den Namespace rz::utils packen
namespace rz {
namespace utils {

// Helper im Namespace (findet jetzt EnvLoader)
static std::string getSecret() {
  QString secret = EnvLoader::get("CAKE_JWT_SECRET", "");
  if (secret.isEmpty()) {
    std::cerr
        << "WARNUNG: CAKE_JWT_SECRET nicht gesetzt! Nutze unsicheren Default."
        << std::endl;
    return "CHANGE_ME_IN_PRODUCTION_THIS_IS_UNSAFE";
  }
  return secret.toStdString();
}

QString TokenUtils::generateToken(const QString &userId, const QString &email,
                                  bool isAdmin) {
  auto now = std::chrono::system_clock::now();

  auto token = jwt::create()
                   .set_issuer("CakePlanner")
                   .set_type("JWS")
                   .set_issued_at(now)
                   .set_expires_at(now + std::chrono::hours(24))
                   .set_payload_claim("uid", jwt::claim(userId.toStdString()))
                   .set_payload_claim("sub", jwt::claim(email.toStdString()))
                   .set_payload_claim("adm", jwt::claim(json_value(isAdmin)))
                   .sign(jwt::algorithm::hs256{getSecret()});

  return QString::fromStdString(token);
}

std::optional<TokenPayload>
TokenUtils::verifyToken(const std::string &rawToken) {
  try {
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{getSecret()})
                        .with_issuer("CakePlanner");

    auto decoded = jwt::decode(rawToken);
    verifier.verify(decoded);

    TokenPayload payload;
    payload.userId =
        QString::fromStdString(decoded.get_payload_claim("uid").as_string());
    payload.email =
        QString::fromStdString(decoded.get_payload_claim("sub").as_string());

    if (decoded.has_payload_claim("adm")) {
      auto claim = decoded.get_payload_claim("adm");
      if (claim.get_type() == jwt::json::type::boolean)
        payload.isAdmin = claim.as_boolean();
      else
        payload.isAdmin = false;
    } else {
      payload.isAdmin = false;
    }

    return payload;

  } catch (const std::exception &e) {
    // std::cerr << "Token Verify Failed: " << e.what() << std::endl;
    return std::nullopt;
  }
}

} // namespace utils
} // namespace rz
