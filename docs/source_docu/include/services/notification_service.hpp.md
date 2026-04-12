# notification_service.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Notification Service |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `notification_service.hpp` |
| **Description** | Notification Service |
| **Version** | 1.2.0 |
| **Date** | 2026-04-11 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz `](#namespace-rz-)
  - [`namespace service `](#namespace-service-)
  - [`class NotificationService `](#class-notificationservice-)
  - [`explicit NotificationService(SmtpService* smtp)`](#explicit-notificationservicesmtpservice-smtp)
  - [`void notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang)`](#void-notifyaccountdeactivatedconst-qstring-email-const-qstring-name-const-qstring-lang)
  - [`void notifyAccountDeleted(const QString& email, const QString& name, const QString& lang)`](#void-notifyaccountdeletedconst-qstring-email-const-qstring-name-const-qstring-lang)
  - [`void notifyPasswordChanged(const QString& email, const QString& name, const QString& lang)`](#void-notifypasswordchangedconst-qstring-email-const-qstring-name-const-qstring-lang)
  - [`void notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang)`](#void-notifyforgotpasswordconst-qstring-email-const-qstring-name-const-qstring-temppassword-const-qstring-lang)
  - [`void sendGroupEmail(const QString& groupId, const QString& text)`](#void-sendgroupemailconst-qstring-groupid-const-qstring-text)
  - [`QString translateText(const QString& text, const QString& targetLang)`](#qstring-translatetextconst-qstring-text-const-qstring-targetlang)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz `

> rz namespace.

---

### `namespace service `

> service namespace.

---

### `class NotificationService `

> NotificationService class.

---

### `explicit NotificationService(SmtpService* smtp)`

> Function implementation.

---

### `void notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang)`

> Function implementation.

---

### `void notifyAccountDeleted(const QString& email, const QString& name, const QString& lang)`

> Function implementation.

---

### `void notifyPasswordChanged(const QString& email, const QString& name, const QString& lang)`

> Function implementation.

---

### `void notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang)`

> Function implementation.

---

### `void sendGroupEmail(const QString& groupId, const QString& text)`

> Sends an email to all members of a group with automatic translation.

| Parameter | Description |
| --- | --- |
| `groupId` | The ID of the group. |
| `text` | The message to send. |

---

### `QString translateText(const QString& text, const QString& targetLang)`

> Translates text using the external translation API.

| Parameter | Description |
| --- | --- |
| `text` | The text to translate. |
| `targetLang` | The target language code (e.g., "de", "en"). |

**Returns:** The translated text.

---

