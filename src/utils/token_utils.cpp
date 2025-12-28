#include "utils/token_utils.hpp"
#include "utils/env_loader.hpp"
#include <chrono>
#include <iostream>

// Wir benötigen den Zugriff auf die JSON-Traits, um Boolesche Werte korrekt zu
// verpacken jwt-cpp nutzt standardmäßig picojson
using json_value = jwt::traits::kazuho_picojson::value_type;

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
                   // FIX 1: Explizite Konvertierung von bool zu picojson::value
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
      // FIX 2: as_boolean() statt as_bool()
      // Der Typ muss picojson::boolean_type sein
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