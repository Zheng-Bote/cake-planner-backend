#include "utils/totp_utils.hpp"
#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <random>
#include <sstream>

// Base32 Alphabet (RFC 4648)
static const char *B32_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

QString TotpUtils::generateSecret() {
  // 160 bit (20 bytes) recommended for TOTP secrets, encoded to Base32
  // Wir nehmen hier 10 Bytes random, was 16 Base32 Zeichen ergibt, oder 20
  // Bytes random = 32 chars. Standard Authenticator Apps mögen oft 16 oder 32
  // Zeichen Strings.

  std::random_device rd;
  std::uniform_int_distribution<int> dist(0, 31);

  QString secret;
  // Generieren wir 32 Zeichen (entspricht 20 Bytes Entropie)
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
      continue; // Ignore non-base32 chars

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
  // Zeit in Big-Endian 8 Byte konvertieren
  uint8_t timeData[8];
  for (int i = 7; i >= 0; --i) {
    timeData[i] = timeStep & 0xFF;
    timeStep >>= 8;
  }

  // HMAC-SHA1 berechnen
  unsigned int len = 0;
  unsigned char *hash = HMAC(EVP_sha1(), keyBytes.data(), keyBytes.size(),
                             timeData, 8, nullptr, &len);

  if (!hash)
    return "";

  // Dynamic Truncation
  int offset = hash[len - 1] & 0x0F;
  int binary = ((hash[offset] & 0x7F) << 24) |
               ((hash[offset + 1] & 0xFF) << 16) |
               ((hash[offset + 2] & 0xFF) << 8) | (hash[offset + 3] & 0xFF);

  int otp = binary % 1000000;

  // Formatieren auf 6 Stellen mit führenden Nullen
  std::ostringstream ss;
  ss << std::setw(6) << std::setfill('0') << otp;
  return QString::fromStdString(ss.str());
}

bool TotpUtils::validateCode(const QString &secret, const QString &code) {
  if (secret.isEmpty() || code.length() != 6)
    return false;

  std::vector<uint8_t> keyBytes = base32Decode(secret);
  int64_t currentStep = getCurrentTimeStep();

  // Wir prüfen: Aktueller Schritt, davor und danach (Toleranz für Zeitdrift)
  for (int i = -1; i <= 1; ++i) {
    if (generateCodeForStep(keyBytes, currentStep + i) == code) {
      return true;
    }
  }
  return false;
}
