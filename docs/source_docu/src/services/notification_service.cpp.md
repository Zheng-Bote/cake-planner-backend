# notification_service.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Notification Service Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `notification_service.cpp` |
| **Description** | Notification Service Implementation |
| **Version** | 1.4.0 |
| **Date** | 2026-04-12 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz `](#namespace-rz-)
  - [`namespace service `](#namespace-service-)
  - [`NotificationService::NotificationService(SmtpService* smtp)`](#notificationservicenotificationservicesmtpservice-smtp)
  - [`void NotificationService::sendGroupEmail(const QString& groupId, const QString& text) `](#void-notificationservicesendgroupemailconst-qstring-groupid-const-qstring-text-)
  - [`QString NotificationService::translateText(const QString& text, const QString& targetLang) `](#qstring-notificationservicetranslatetextconst-qstring-text-const-qstring-targetlang-)
  - [`std::vector<QString> NotificationService::getGlobalAdminEmails() `](#stdvectorqstring-notificationservicegetglobaladminemails-)
  - [`QSqlQuery query(db)`](#qsqlquery-querydb)
  - [`void NotificationService::notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail) `](#void-notificationservicenotifyadminsnewuserconst-qstring-newusername-const-qstring-newuseremail-)
  - [`void NotificationService::notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn, const QByteArray& icsData) `](#void-notificationservicenotifygroupneweventconst-qstring-groupname-const-qstring-bakername-const-qstring-date-const-stdvectorqstring-recipientsde-const-stdvectorqstring-recipientsen-const-qbytearray-icsdata-)
  - [`void NotificationService::notifyAccountActivated(const QString& email, const QString& name, const QString& lang) `](#void-notificationservicenotifyaccountactivatedconst-qstring-email-const-qstring-name-const-qstring-lang-)
  - [`void NotificationService::notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang) `](#void-notificationservicenotifyaccountdeactivatedconst-qstring-email-const-qstring-name-const-qstring-lang-)
  - [`void NotificationService::notifyAccountDeleted(const QString& email, const QString& name, const QString& lang) `](#void-notificationservicenotifyaccountdeletedconst-qstring-email-const-qstring-name-const-qstring-lang-)
  - [`void NotificationService::notifyPasswordChanged(const QString& email, const QString& name, const QString& lang) `](#void-notificationservicenotifypasswordchangedconst-qstring-email-const-qstring-name-const-qstring-lang-)
  - [`void NotificationService::notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang) `](#void-notificationservicenotifyforgotpasswordconst-qstring-email-const-qstring-name-const-qstring-temppassword-const-qstring-lang-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz `

> rz namespace.

---

### `namespace service `

> service namespace.

---

### `NotificationService::NotificationService(SmtpService* smtp)`

> Constructs the NotificationService.

| Parameter | Description |
| --- | --- |
| `smtp` | Pointer to the SMTP service used for sending emails. |

---

### `void NotificationService::sendGroupEmail(const QString& groupId, const QString& text) `

> Sends an email to all members of a group with automatic translation.

| Parameter | Description |
| --- | --- |
| `groupId` | The ID of the group. |
| `text` | The original message to translate and send. |

---

### `QString NotificationService::translateText(const QString& text, const QString& targetLang) `

> Translates text using an external translation API.

| Parameter | Description |
| --- | --- |
| `text` | The text to translate. |
| `targetLang` | The target language code (e.g., "de", "en"). |

**Returns:** The translated text, or the original text if translation fails.

---

### `std::vector<QString> NotificationService::getGlobalAdminEmails() `

> Retrieves the email addresses of all global administrators.

**Returns:** A vector of admin email addresses.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `void NotificationService::notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail) `

> Notifies admins about a new user registration.

| Parameter | Description |
| --- | --- |
| `newUserName` | The name of the new user. |
| `newUserEmail` | The email of the new user. |

---

### `void NotificationService::notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn, const QByteArray& icsData) `

> Notifies group members about a new event (e.g., someone bringing cake).

| Parameter | Description |
| --- | --- |
| `groupName` | The name of the group. |
| `bakerName` | The name of the user bringing the cake. |
| `date` | The date of the event. |
| `recipientsDe` | List of email addresses for German notifications. |
| `recipientsEn` | List of email addresses for English notifications. |

---

### `void NotificationService::notifyAccountActivated(const QString& email, const QString& name, const QString& lang) `

> Notifies a user that their account has been activated.

| Parameter | Description |
| --- | --- |
| `email` | The user's email address. |
| `name` | The user's name. |
| `lang` | The user's preferred language ("de" or "en"). |

---

### `void NotificationService::notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang) `

> Notifies a user that their account has been deactivated.

| Parameter | Description |
| --- | --- |
| `email` | The user's email address. |
| `name` | The user's name. |
| `lang` | The user's preferred language ("de" or "en"). |

---

### `void NotificationService::notifyAccountDeleted(const QString& email, const QString& name, const QString& lang) `

> Notifies a user that their account has been deleted.

| Parameter | Description |
| --- | --- |
| `email` | The user's email address. |
| `name` | The user's name. |
| `lang` | The user's preferred language ("de" or "en"). |

---

### `void NotificationService::notifyPasswordChanged(const QString& email, const QString& name, const QString& lang) `

> Notifies a user that their password has been changed.

| Parameter | Description |
| --- | --- |
| `email` | The user's email address. |
| `name` | The user's name. |
| `lang` | The user's preferred language ("de" or "en"). |

---

### `void NotificationService::notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang) `

> Sends a temporary password to the user.

| Parameter | Description |
| --- | --- |
| `email` | The user's email address. |
| `name` | The user's name. |
| `tempPassword` | The temporary password. |
| `lang` | The user's preferred language. |

---

