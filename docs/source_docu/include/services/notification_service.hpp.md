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
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
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

