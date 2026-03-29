# auth_controller.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Auth Controller Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `auth_controller.cpp` |
| **Description** | Auth Controller Implementation |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz `](#namespace-rz-)
  - [`namespace controller `](#namespace-controller-)
  - [`AuthController::AuthController(service::NotificationService* notifyService)`](#authcontrollerauthcontrollerservicenotificationservice-notifyservice)
  - [`void AuthController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app) `](#void-authcontrollerregisterroutescrowapprzmiddlewareauthmiddleware-app-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz `

> rz namespace.

---

### `namespace controller `

> controller namespace.

---

### `AuthController::AuthController(service::NotificationService* notifyService)`

> Constructs the AuthController.

| Parameter | Description |
| --- | --- |
| `notifyService` | Pointer to the notification service (can be nullptr). |

---

### `void AuthController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app) `

> Registers authentication-related routes with the Crow application.

| Parameter | Description |
| --- | --- |
| `app` | The Crow application instance to register routes with. |

---

