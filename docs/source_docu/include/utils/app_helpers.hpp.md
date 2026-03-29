# app_helpers.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Application Helpers Header |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `app_helpers.hpp` |
| **Description** | Application Helpers Header |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz::utils `](#namespace-rzutils-)
  - [`inline void ensureDirectoryExists(const std::string& path) `](#inline-void-ensuredirectoryexistsconst-stdstring-path-)
  - [`namespace fs = std::filesystem`](#namespace-fs--stdfilesystem)
  - [`inline crow::json::wvalue makeErrorJson(int code, const std::string& message) `](#inline-crowjsonwvalue-makeerrorjsonint-code-const-stdstring-message-)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace rz::utils `

> rz namespace.

---

### `inline void ensureDirectoryExists(const std::string& path) `

> Ensures that a directory exists, creates it if necessary.

---

### `namespace fs = std::filesystem`

> fs namespace.

---

### `inline crow::json::wvalue makeErrorJson(int code, const std::string& message) `

> Creates a standardized JSON error response object.

---

