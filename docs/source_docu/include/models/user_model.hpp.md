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
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`struct User `](#struct-user-)
  - [`struct GroupMembership `](#struct-groupmembership-)
  - [`static bool setMustChangePassword(const QString &userId, bool mustChange)`](#static-bool-setmustchangepasswordconst-qstring-userid-bool-mustchange)
  - [`bool setTempPassword(const QString &hash, int durationInHours = 24)`](#bool-settemppasswordconst-qstring-hash-int-durationinhours--24)
  - [`bool clearTempPassword()`](#bool-cleartemppassword)
  - [`bool enable2FA(const QString &secret)`](#bool-enable2faconst-qstring-secret)
  - [`static std::optional<User> getByEmail(const QString &email)`](#static-stdoptionaluser-getbyemailconst-qstring-email)
  - [`static std::vector<User> getAll(const QString &filterGroupId = "")`](#static-stdvectoruser-getallconst-qstring-filtergroupid--)
  - [`static std::pair<QString, QString> getGroupAndRole(const QString &userId)`](#static-stdpairqstring-qstring-getgroupandroleconst-qstring-userid)
  - [`static bool touchLastLogin(const QString &userId)`](#static-bool-touchlastloginconst-qstring-userid)
  - [`static bool updateStatus(const QString &userId, bool isActive)`](#static-bool-updatestatusconst-qstring-userid-bool-isactive)
  - [`static bool updatePassword(const QString &userId, const QString &newHash)`](#static-bool-updatepasswordconst-qstring-userid-const-qstring-newhash)
  - [`static bool updateLanguage(const QString& userId, const QString& lang)`](#static-bool-updatelanguageconst-qstring-userid-const-qstring-lang)
  - [`static bool updateEmailLanguage(const QString& userId, const QString& lang)`](#static-bool-updateemaillanguageconst-qstring-userid-const-qstring-lang)
  - [`static bool assignToGroup(const QString &userId, const QString &groupId)`](#static-bool-assigntogroupconst-qstring-userid-const-qstring-groupid)
  - [`static bool setGroupRole(const QString &userId, const QString &groupId, const QString &role)`](#static-bool-setgrouproleconst-qstring-userid-const-qstring-groupid-const-qstring-role)
  - [`static QString getGroupRole(const QString &userId, const QString &groupId)`](#static-qstring-getgrouproleconst-qstring-userid-const-qstring-groupid)
  - [`static std::vector<GroupMembership> getGroupsForUser(const QString &userId)`](#static-stdvectorgroupmembership-getgroupsforuserconst-qstring-userid)
  - [`static bool deleteGroup(const QString &groupId)`](#static-bool-deletegroupconst-qstring-groupid)
  - [`static bool softDelete(const QString& userId)`](#static-bool-softdeleteconst-qstring-userid)
  - [`static bool existsAnyAdmin()`](#static-bool-existsanyadmin)
  - [`bool create()`](#bool-create)

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

