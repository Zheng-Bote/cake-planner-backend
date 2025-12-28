#pragma once
#include <QString>
#include <string>

class PasswordUtils {
public:
  /**
   * Erstellt einen Argon2id Hash.
   * Rückgabeformat: $argon2id$v=19$m=65536,t=3,p=4$...salt...$hash...
   */
  static QString hashPassword(const QString &plainText);

  /**
   * Verifiziert ein Passwort gegen einen Argon2 Hash String.
   */
  static bool verifyPassword(const QString &plainText,
                             const QString &encodedHash);
};