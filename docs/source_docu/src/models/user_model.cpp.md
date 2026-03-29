# user_model.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | User Model Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `user_model.cpp` |
| **Description** | User Model Implementation |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `std::pair<QString, QString> User::getGroupAndRole(const QString &userId) `

> Retrieves the group ID and role for a specific user.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |

**Returns:** A pair containing the group ID (first) and the role (second).

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `crow::json::wvalue User::toJson() const `

> Converts the User object to a JSON value.

**Returns:** A crow::json::wvalue containing the user data.

---

### `std::optional<User> User::getByEmail(const QString &email) `

> Retrieves a user by their email address.

| Parameter | Description |
| --- | --- |
| `email` | The email address to search for. |

**Returns:** An optional User object if found, std::nullopt otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `std::optional<User> User::getById(const QString &id) `

> Retrieves a user by their unique ID.

| Parameter | Description |
| --- | --- |
| `id` | The ID of the user. |

**Returns:** An optional User object if found, std::nullopt otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `std::vector<User> User::getAll(const QString &filterGroupId) `

> Retrieves all users, optionally filtered by group.

| Parameter | Description |
| --- | --- |
| `filterGroupId` | Optional group ID to filter users by. |

**Returns:** A vector of User objects.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::existsAnyAdmin() `

> Checks if any admin user exists in the database.

**Returns:** True if at least one admin exists, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::create() `

> Creates a new user in the database.

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::enable2FA(const QString &secret) `

> Enables Two-Factor Authentication (2FA) for the user.

| Parameter | Description |
| --- | --- |
| `secret` | The TOTP secret key. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::updateStatus(const QString &userId, bool isActive) `

> Updates the active status of a user.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `isActive` | The new active status. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::setMustChangePassword(const QString &userId, bool mustChange) `

> Sets the "must change password" flag for a user.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `mustChange` | True if the user must change their password, false otherwise. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::updatePassword(const QString &userId, const QString &newHash) `

> Updates the user's password hash.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `newHash` | The new password hash. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::setTempPassword(const QString &hash, int durationInHours) `

> Sets a temporary password for the user.

| Parameter | Description |
| --- | --- |
| `hash` | The hash of the temporary password. |
| `durationInHours` | How long the password is valid. |

**Returns:** True if successful.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::clearTempPassword() `

> Clears the temporary password for the user.

**Returns:** True if successful.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `/**`

> Updates the last login timestamp for a user.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `std::vector<std::pair<QString, QString>> User::getAllGroups() `

> Retrieves all existing user groups.

**Returns:** A vector of pairs, where each pair contains the group ID and group name.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::assignToGroup(const QString &userId, const QString &groupId) `

> Assigns a user to a group.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `groupId` | The ID of the group. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::setGroupRole(const QString &userId, const QString &groupId,`

> Sets the role of a user within a group.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `groupId` | The ID of the group. |
| `role` | The new role (e.g., "admin", "member"). |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `QString User::getGroupRole(const QString &userId, const QString &groupId) `

> Retrieves the role of a user within a specific group.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `groupId` | The ID of the group. |

**Returns:** The role as a string, or an empty string if not found.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::softDelete(const QString& userId) `

> Soft-deletes a user account.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user to delete. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::updateEmailLanguage(const QString& userId, const QString& lang) `

> Updates the preferred language for email notifications.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `lang` | The language code (e.g., "en", "de"). |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::updateLanguage(const QString& userId, const QString& lang) `

> Updates the preferred UI language.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user. |
| `lang` | The language code (e.g., "en", "de"). |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `QString User::createGroup(const QString &name) `

> Creates a new group in the database.

| Parameter | Description |
| --- | --- |
| `name` | The name of the group. |

**Returns:** The new group ID (UUID) or an empty string on error.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool User::deleteGroup(const QString &groupId) `

> Deletes a group from the database.

| Parameter | Description |
| --- | --- |
| `groupId` | The ID of the group to delete. |

**Returns:** True if deleted, false if the group is not empty or on DB error.

---

### `std::vector<User::GroupMembership> User::getGroupsForUser(const QString &userId) `

> Function implementation.

---

### `QSqlQuery query(db)`

> Function implementation.

---

