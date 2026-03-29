# system_controller.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Implementation of system routes |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `system_controller.cpp` |
| **Description** | Implementation of system routes |
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
  - [`void SystemController::registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app) `](#void-systemcontrollerregisterroutescrowapprzmiddlewareauthmiddleware-app-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz::controller `

> rz namespace.

---

### `void SystemController::registerRoutes(crow::App<rz::middleware::AuthMiddleware>& app) `

> Registers system-related routes with the Crow application.

| Parameter | Description |
| --- | --- |
| `app` | The Crow application instance to register routes with. |

---

