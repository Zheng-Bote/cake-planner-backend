#pragma once
#include <QString>
#include <jwt-cpp/jwt.h>
#include <optional>

struct TokenPayload {
  QString userId;
  QString email;
  bool isAdmin;
};

class TokenUtils {
public:
  // Generiert ein Token, gültig für 24h
  static QString generateToken(const QString &userId, const QString &email,
                               bool isAdmin);

  // Verifiziert das Token und gibt Payload zurück (oder nullopt bei Fehler)
  static std::optional<TokenPayload> verifyToken(const std::string &rawToken);
};