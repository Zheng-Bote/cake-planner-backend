# password_utils.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Password Hashing Utilities |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `password_utils.cpp` |
| **Description** | Password Hashing Utilities |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace utils `](#namespace-utils-)
  - [`QString PasswordUtils::hashPassword(const QString &plainText) `](#qstring-passwordutilshashpasswordconst-qstring-plaintext-)
  - [`std::uniform_int_distribution<uint8_t> dist(0, 255)`](#stduniform_int_distributionuint8_t-dist0-255)
  - [`std::vector<char> encoded(encodedLen)`](#stdvectorchar-encodedencodedlen)
  - [`bool PasswordUtils::verifyPassword(const QString &plainText,`](#bool-passwordutilsverifypasswordconst-qstring-plaintext)
  - [`QString PasswordUtils::generateRandomPassword(int length) `](#qstring-passwordutilsgeneraterandompasswordint-length-)
  - [`std::uniform_int_distribution<size_t> distribution(0, max_index - 1)`](#stduniform_int_distributionsize_t-distribution0-max_index---1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace utils `

> utils namespace.

---

### `QString PasswordUtils::hashPassword(const QString &plainText) `

> Hashes a plain text password using Argon2id.

| Parameter | Description |
| --- | --- |
| `plainText` | The password to hash. |

**Returns:** The encoded hash string (including salt and parameters) or empty string on failure.

---

### `std::uniform_int_distribution<uint8_t> dist(0, 255)`

> Function implementation.

---

### `std::vector<char> encoded(encodedLen)`

> Function implementation.

---

### `bool PasswordUtils::verifyPassword(const QString &plainText,`

> Verifies a password against an encoded hash.

| Parameter | Description |
| --- | --- |
| `plainText` | The plain text password to verify. |
| `encodedHash` | The Argon2id encoded hash to verify against. |

**Returns:** True if the password matches, false otherwise.

---

### `QString PasswordUtils::generateRandomPassword(int length) `

> Generates a random password.

| Parameter | Description |
| --- | --- |
| `length` | Length of the password. |

**Returns:** Random string.

---

### `std::uniform_int_distribution<size_t> distribution(0, max_index - 1)`

> Function implementation.

---

