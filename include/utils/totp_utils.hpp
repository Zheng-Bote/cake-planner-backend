/**
 * @file totp_utils.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief TOTP Utilities (Google Authenticator compatible)
 * @version 0.2.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <QString>
#include <cstdint>
#include <vector>

namespace rz {
namespace utils {

class TotpUtils {
public:
  // Generiert ein zufälliges 16-Byte Secret (Base32 encoded)
  static QString generateSecret();

  // Erstellt die URL für den QR-Code (otpauth://totp/...)
  // Default Parameter 'issuer' muss im Header stehen
  static QString getProvisioningUri(const QString &userEmail,
                                    const QString &secret,
                                    const QString &issuer = "CakePlanner");

  // Prüft, ob der Code für das gegebene Secret gültig ist (toleriert +- 1 Zeitfenster)
  static bool validateCode(const QString &secret, const QString &code);

private:
  // WICHTIG: Diese privaten Helper müssen hier deklariert bleiben
  static std::vector<uint8_t> base32Decode(const QString &secret);
  static int64_t getCurrentTimeStep();
  static QString generateCodeForStep(const std::vector<uint8_t> &keyBytes,
                                     int64_t timeStep);
};

} // namespace utils
} // namespace rz
