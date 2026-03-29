# token_utils.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | JWT Token Utilities Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `token_utils.cpp` |
| **Description** | JWT Token Utilities Implementation |
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

### `QString TokenUtils::generateToken(const QString &userId, const QString &email,`

> Generates a JWT (JSON Web Token) for a user.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `email` | The email address of the user. |
| `isAdmin` | True if the user is an administrator. |

**Returns:** The generated JWT as a string.

---

### `std::optional<TokenPayload>`

> Verifies and decodes a JWT.

| Parameter | Description |
| --- | --- |
| `rawToken` | The raw JWT string. |

**Returns:** An optional TokenPayload containing the user info if verification succeeds, std::nullopt otherwise.

---

