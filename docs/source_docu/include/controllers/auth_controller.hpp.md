# auth_controller.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Auth Controller with Notification Service injection |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `auth_controller.hpp` |
| **Description** | Auth Controller with Notification Service injection |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace service `](#namespace-service-)
  - [`namespace controller `](#namespace-controller-)
  - [`class AuthController `](#class-authcontroller-)
  - [`explicit AuthController(service::NotificationService* notifyService)`](#explicit-authcontrollerservicenotificationservice-notifyservice)
  - [`// CORRECTED: Fully qualified namespace for AuthMiddleware`](#-corrected-fully-qualified-namespace-for-authmiddleware)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace service `

> service namespace.

---

### `namespace controller `

> controller namespace.

---

### `class AuthController `

> AuthController class.

---

### `explicit AuthController(service::NotificationService* notifyService)`

> Construct a new Auth Controller

| Parameter | Description |
| --- | --- |
| `notifyService` | Pointer to the notification service |

---

### `// CORRECTED: Fully qualified namespace for AuthMiddleware`

> Register authentication routes

| Parameter | Description |
| --- | --- |
| `app` | Crow App instance |

---

