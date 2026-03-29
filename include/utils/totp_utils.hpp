/**
 * SPDX-FileComment: TOTP Utilities (Google Authenticator compatible)
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file totp_utils.hpp
 * @brief TOTP Utilities (Google Authenticator compatible)
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once
#include <QString>
#include <cstdint>
#include <vector>

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief utils namespace.
 */
namespace utils {

/**
 * @brief TotpUtils class.
 */
class TotpUtils {
public:
  // Generates a random 16-byte secret (Base32 encoded)
  static QString generateSecret();

  // Creates the URL for the QR code (otpauth://totp/...)
  // Default parameter 'issuer' must be in the header
  static QString getProvisioningUri(const QString &userEmail,
                                    const QString &secret,
                                    const QString &issuer = "CakePlanner");

  // Checks if the code is valid for the given secret (tolerates +- 1 time step)
  static bool validateCode(const QString &secret, const QString &code);

private:
  // IMPORTANT: These private helpers must remain declared here
  static std::vector<uint8_t> base32Decode(const QString &secret);
  /**
   * @brief Function implementation.
   */
  static int64_t getCurrentTimeStep();
  static QString generateCodeForStep(const std::vector<uint8_t> &keyBytes,
                                     int64_t timeStep);
};

} // namespace utils
} // namespace rz
