# user_controller.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | User Controller with Email Notifications |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `user_controller.cpp` |
| **Description** | User Controller with Email Notifications |
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

### `namespace controller `

> controller namespace.

---

### `void UserController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) `

> Registers user-related routes with the Crow application.

| Parameter | Description |
| --- | --- |
| `app` | The Crow application instance to register routes with. |
| `notifyService` | Pointer to the notification service for email notifications. |

---

