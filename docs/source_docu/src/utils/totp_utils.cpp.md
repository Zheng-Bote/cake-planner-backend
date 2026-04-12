# totp_utils.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Time-based One-Time Password (TOTP) Utilities |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `totp_utils.cpp` |
| **Description** | Time-based One-Time Password (TOTP) Utilities |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace utils `

> utils namespace.

---

### `QString TotpUtils::generateSecret() `

> Generates a random Base32 secret for TOTP.

**Returns:** The generated secret string.

---

### `std::uniform_int_distribution<int> dist(0, 31)`

> Function implementation.

---

### `QString TotpUtils::getProvisioningUri(const QString &userEmail,`

> Generates the provisioning URI for the QR code.

| Parameter | Description |
| --- | --- |
| `userEmail` | The email address of the user. |
| `secret` | The Base32 secret. |
| `issuer` | The name of the issuer (e.g., "Cake Planner"). |

**Returns:** The provisioning URI string.

---

### `int64_t TotpUtils::getCurrentTimeStep() `

> Gets the current time step for TOTP calculation.

**Returns:** The current time step.

---

### `std::vector<uint8_t> TotpUtils::base32Decode(const QString &secret) `

> Decodes a Base32 string into bytes.

| Parameter | Description |
| --- | --- |
| `secret` | The Base32 encoded secret. |

**Returns:** A vector of bytes representing the decoded secret.

---

### `QString TotpUtils::generateCodeForStep(const std::vector<uint8_t> &keyBytes,`

> Generates a TOTP code for a specific time step.

| Parameter | Description |
| --- | --- |
| `keyBytes` | The decoded secret key in bytes. |
| `timeStep` | The time step to generate the code for. |

**Returns:** The 6-digit TOTP code as a string.

---

### `bool TotpUtils::validateCode(const QString &secret, const QString &code) `

> Validates a TOTP code against a secret.

| Parameter | Description |
| --- | --- |
| `secret` | The Base32 secret. |
| `code` | The TOTP code to validate. |

**Returns:** True if the code is valid, false otherwise.

---

