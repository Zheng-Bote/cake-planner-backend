/**
 * @file totp_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Time-based One-Time Password (TOTP) Utilities
 * @version 0.1.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
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

QString TotpUtils::generateSecret() {
  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 31);

  QString secret;
  for (int i = 0; i < 32; ++i) {
    secret.append(B32_CHARS[dist(rd)]);
  }
  return secret;
}

QString TotpUtils::getProvisioningUri(const QString &userEmail,
                                      const QString &secret,
                                      const QString &issuer) {
  return QString("otpauth://totp/"
                 "%1:%2?secret=%3&issuer=%1&algorithm=SHA1&digits=6&period=30")
      .arg(issuer, userEmail, secret);
}

int64_t TotpUtils::getCurrentTimeStep() {
  return QDateTime::currentSecsSinceEpoch() / 30;
}

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
