/**
 * @file password_utils.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

// (OWASP Empfehlungen).
// Time Cost(t) : 3 Iterationen
// Memory Cost(m) : 64 MB(65536 KB)
// Parallelism(p) : 4 Threads

#include "utils/password_utils.hpp"
#include "argon2.h"
#include <QByteArray>
#include <QDebug>
#include <QUuid>
#include <random>
#include <vector>

// Interne Konstanten für Argon2id Sicherheit
const uint32_t T_COST = 3;      // 3 Iterationen
const uint32_t M_COST = 65536;  // 64 MiB RAM Nutzung
const uint32_t PARALLELISM = 4; // 4 Lanes
const uint32_t SALT_LEN = 16;   // 16 Bytes Salt
const uint32_t HASH_LEN = 32;   // 32 Bytes Output Hash

QString PasswordUtils::hashPassword(const QString &plainText) {
  // 1. Zufälligen Salt generieren (kryptographisch sicher)
  uint8_t salt[SALT_LEN];
  // Wir nutzen std::random_device für OS-seitige Entropie
  std::random_device rd;
  std::uniform_int_distribution<uint8_t> dist(0, 255);
  for (uint32_t i = 0; i < SALT_LEN; ++i) {
    salt[i] = dist(rd);
  }

  // Puffer für den encodierten String
  // Argon2 braucht Platz für Parameter, Salt und Hash im String-Format
  size_t encodedLen = argon2_encodedlen(T_COST, M_COST, PARALLELISM, SALT_LEN,
                                        HASH_LEN, Argon2_id);
  std::vector<char> encoded(encodedLen);

  QByteArray pwdBytes = plainText.toUtf8();

  // 2. Argon2id ausführen
  int result = argon2id_hash_encoded(
      T_COST, M_COST, PARALLELISM, pwdBytes.data(), pwdBytes.length(), salt,
      SALT_LEN, HASH_LEN, encoded.data(), encodedLen);

  if (result != ARGON2_OK) {
    qCritical() << "Argon2 Hashing fehlgeschlagen, Error Code:" << result;
    return QString();
  }

  return QString::fromLatin1(encoded.data());
}

bool PasswordUtils::verifyPassword(const QString &plainText,
                                   const QString &encodedHash) {
  if (encodedHash.isEmpty())
    return false;

  QByteArray pwdBytes = plainText.toUtf8();
  std::string hashStr = encodedHash.toStdString();

  // Argon2 verifiziert direkt gegen den encoded String (der Salt & Params
  // enthält)
  int result =
      argon2id_verify(hashStr.c_str(), pwdBytes.data(), pwdBytes.length());

  return (result == ARGON2_OK);
}