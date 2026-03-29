# env_loader.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Environment Loader Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `env_loader.cpp` |
| **Description** | Environment Loader Implementation |
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

### `namespace utils `

> utils namespace.

---

### `void EnvLoader::load(const std::string &filenameStd) `

> Loads environment variables from a file.

| Parameter | Description |
| --- | --- |
| `filenameStd` | The name of the file to load (default: .env). |

---

### `QFile file(foundPath)`

> Function implementation.

---

### `QTextStream in(&file)`

> Function implementation.

---

### `QString EnvLoader::get(const std::string &keyStd, const std::string &defaultValueStd) `

> Retrieves an environment variable as a QString.

| Parameter | Description |
| --- | --- |
| `keyStd` | The key of the environment variable. |
| `defaultValueStd` | The default value if the key is not found. |

**Returns:** The value of the environment variable or the default value.

---

### `std::string EnvLoader::getString(const std::string &keyStd, const std::string &defaultValueStd) `

> Retrieves an environment variable as a std::string.

| Parameter | Description |
| --- | --- |
| `keyStd` | The key of the environment variable. |
| `defaultValueStd` | The default value if the key is not found. |

**Returns:** The value of the environment variable or the default value.

---

### `int EnvLoader::getInt(const std::string &keyStd, int defaultValue) `

> Retrieves an environment variable as an integer.

| Parameter | Description |
| --- | --- |
| `keyStd` | The key of the environment variable. |
| `defaultValue` | The default value if the key is not found or not a valid number. |

**Returns:** The integer value of the environment variable or the default value.

---

