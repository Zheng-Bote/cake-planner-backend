#pragma once
#include <QString>
#include <cstdint>
#include <vector>

class TotpUtils {
public:
  // Generiert ein zufälliges 16-Byte Secret (Base32 encoded)
  static QString generateSecret();

  // Erstellt die URL für den QR-Code (otpauth://totp/...)
  static QString getProvisioningUri(const QString &userEmail,
                                    const QString &secret,
                                    const QString &issuer = "CakePlanner");

  // Prüft, ob der Code für das gegebene Secret gültig ist (toleriert +- 1
  // Zeitfenster)
  static bool validateCode(const QString &secret, const QString &code);

private:
  static std::vector<uint8_t> base32Decode(const QString &secret);
  static int64_t getCurrentTimeStep();
  static QString generateCodeForStep(const std::vector<uint8_t> &keyBytes,
                                     int64_t timeStep);
};
