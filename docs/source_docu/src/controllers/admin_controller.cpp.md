# admin_controller.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Admin Controller Implementation with Group Management |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `admin_controller.cpp` |
| **Description** | Admin Controller Implementation with Group Management |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz::controller `](#namespace-rzcontroller-)
  - [`void AdminController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) `](#void-admincontrollerregisterroutescrowapprzmiddlewareauthmiddleware-app-servicenotificationservice-notifyservice-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz::controller `

> rz namespace.

---

### `void AdminController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) `

> Registers all admin-related routes with the Crow application.

| Parameter | Description |
| --- | --- |
| `app` | The Crow application instance to register routes with. |
| `notifyService` | Pointer to the notification service for sending emails (can be nullptr). |

---

