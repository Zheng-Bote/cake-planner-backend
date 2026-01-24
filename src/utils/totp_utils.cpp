/**
 * @file totp_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Time-based One-Time Password (TOTP) Utilities
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils/totp_utils.hpp"
#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <random>
#include <sstream>
#include <cstring>

// Base32 Alphabet (RFC 4648)
static const char *B32_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// Namespace rz::utils
namespace rz {
namespace utils {

/**
 * @brief Generates a random Base32 secret for TOTP.
 *
 * The secret is 32 characters long, consisting of upper-case letters A-Z and digits 2-7.
 *
 * @return The generated secret string.
 */
QString TotpUtils::generateSecret() {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 31);

  QString secret;
  for (int i = 0; i < 32; ++i) {
    secret.append(B32_CHARS[dist(rd)]);
  }
  return secret;
}

/**
 * @brief Generates the provisioning URI for the QR code.
 *
 * The URI follows the otpauth scheme (RFC 6238).
 *
 * @param userEmail The email address of the user.
 * @param secret The Base32 secret.
 * @param issuer The name of the issuer (e.g., "Cake Planner").
 * @return The provisioning URI string.
 */
QString TotpUtils::getProvisioningUri(const QString &userEmail,
                                      const QString &secret,
                                      const QString &issuer) {
  return QString("otpauth://totp/"
                 "%1:%2?secret=%3&issuer=%1&algorithm=SHA1&digits=6&period=30")
      .arg(issuer, userEmail, secret);
}

/**
 * @brief Gets the current time step for TOTP calculation.
 *
 * TOTP uses 30-second intervals. This function returns the number of 30-second
 * intervals that have elapsed since the Unix epoch.
 *
 * @return The current time step.
 */
int64_t TotpUtils::getCurrentTimeStep() {
  return QDateTime::currentSecsSinceEpoch() / 30;
}

/**
 * @brief Decodes a Base32 string into bytes.
 *
 * @param secret The Base32 encoded secret.
 * @return A vector of bytes representing the decoded secret.
 */
std::vector<uint8_t> TotpUtils::base32Decode(const QString &secret) {
  std::vector<uint8_t> result;
  unsigned int buffer = 0;
  int bitsLeft = 0;

  for (QChar c : secret.toUpper()) {
    const char *p = strchr(B32_CHARS, c.toLatin1());
    if (!p)
      continue;

    buffer = (buffer << 5) | (p - B32_CHARS);
    bitsLeft += 5;

    if (bitsLeft >= 8) {
      result.push_back((buffer >> (bitsLeft - 8)) & 0xFF);
      bitsLeft -= 8;
    }
  }
  return result;
}

/**
 * @brief Generates a TOTP code for a specific time step.
 *
 * Calculates the HMAC-SHA1 hash of the time step using the secret key,
 * and extracts a 6-digit code.
 *
 * @param keyBytes The decoded secret key in bytes.
 * @param timeStep The time step to generate the code for.
 * @return The 6-digit TOTP code as a string.
 */
QString TotpUtils::generateCodeForStep(const std::vector<uint8_t> &keyBytes,
                                       int64_t timeStep) {
  uint8_t timeData[8];
  for (int i = 7; i >= 0; --i) {
    timeData[i] = timeStep & 0xFF;
    timeStep >>= 8;
  }

  unsigned int len = 0;
  unsigned char *hash = HMAC(EVP_sha1(), keyBytes.data(), keyBytes.size(),
                             timeData, 8, nullptr, &len);

  if (!hash) return "";

  int offset = hash[len - 1] & 0x0F;
  int binary = ((hash[offset] & 0x7F) << 24) |
               ((hash[offset + 1] & 0xFF) << 16) |
               ((hash[offset + 2] & 0xFF) << 8) | (hash[offset + 3] & 0xFF);

  int otp = binary % 1000000;

  std::ostringstream ss;
  ss << std::setw(6) << std::setfill('0') << otp;
  return QString::fromStdString(ss.str());
}

/**
 * @brief Validates a TOTP code against a secret.
 *
 * Checks the code for the current time step and the immediately preceding and
 * succeeding time steps (to account for clock drift).
 *
 * @param secret The Base32 secret.
 * @param code The TOTP code to validate.
 * @return True if the code is valid, false otherwise.
 */
bool TotpUtils::validateCode(const QString &secret, const QString &code) {
  if (secret.isEmpty() || code.length() != 6) return false;

  std::vector<uint8_t> keyBytes = base32Decode(secret);
  int64_t currentStep = getCurrentTimeStep();

  for (int i = -1; i <= 1; ++i) {
    if (generateCodeForStep(keyBytes, currentStep + i) == code) {
      return true;
    }
  }
  return false;
}

} // namespace utils
} // namespace rz
