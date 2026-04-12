# user_model.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | User Model with Group Management Extensions |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `user_model.hpp` |
| **Description** | User Model with Group Management Extensions |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `struct User `

> User struct.

---

### `struct GroupMembership `

> GroupMembership struct.

---

### `static bool setMustChangePassword(const QString &userId, bool mustChange)`

> Function implementation.

---

### `bool setTempPassword(const QString &hash, int durationInHours = 24)`

> Function implementation.

---

### `bool clearTempPassword()`

> Function implementation.

---

### `bool enable2FA(const QString &secret)`

> Function implementation.

---

### `static std::optional<User> getByEmail(const QString &email)`

> Function implementation.

---

### `static std::vector<User> getAll(const QString &filterGroupId = "")`

> Function implementation.

---

### `static std::pair<QString, QString> getGroupAndRole(const QString &userId)`

> Function implementation.

---

### `static bool touchLastLogin(const QString &userId)`

> Function implementation.

---

### `static bool updateStatus(const QString &userId, bool isActive)`

> Function implementation.

---

### `static bool updatePassword(const QString &userId, const QString &newHash)`

> Function implementation.

---

### `static bool updateLanguage(const QString& userId, const QString& lang)`

> Function implementation.

---

### `static bool updateEmailLanguage(const QString& userId, const QString& lang)`

> Function implementation.

---

### `static bool updateReceiveEventEmails(const QString& userId, bool receive)`

> Function implementation.

---

### `static bool assignToGroup(const QString &userId, const QString &groupId)`

> Function implementation.

---

### `static bool setGroupRole(const QString &userId, const QString &groupId, const QString &role)`

> Function implementation.

---

### `static QString getGroupRole(const QString &userId, const QString &groupId)`

> Function implementation.

---

### `static std::vector<GroupMembership> getGroupsForUser(const QString &userId)`

> Function implementation.

---

### `static bool deleteGroup(const QString &groupId)`

> Function implementation.

---

### `static bool softDelete(const QString& userId)`

> Function implementation.

---

### `static bool existsAnyAdmin()`

> Function implementation.

---

### `bool create()`

> Function implementation.

---

