# auth_middleware.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Auth Middleware |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `auth_middleware.hpp` |
| **Description** | Auth Middleware |
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
  - [`namespace middleware `](#namespace-middleware-)
  - [`struct AuthMiddleware `](#struct-authmiddleware-)
  - [`void before_handle(crow::request &req, crow::response &res, context &ctx) `](#void-before_handlecrowrequest-req-crowresponse-res-context-ctx-)
  - [`void after_handle(crow::request &req, crow::response &res, context &ctx) `](#void-after_handlecrowrequest-req-crowresponse-res-context-ctx-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz `

> rz namespace.

---

### `namespace middleware `

> middleware namespace.

---

### `struct AuthMiddleware `

> AuthMiddleware struct.

---

### `void before_handle(crow::request &req, crow::response &res, context &ctx) `

> Function implementation.

---

### `void after_handle(crow::request &req, crow::response &res, context &ctx) `

> Function implementation.

---

