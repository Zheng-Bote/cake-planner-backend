# config_model.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Configuration Model to load env vars |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `ConfigModel.hpp` |
| **Description** | Configuration Model to load env vars |
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

### `namespace model `

> model namespace.

---

### `class ConfigModel `

> The ConfigModel class handles loading and providing access to configuration settings.

---

### `ConfigModel()`

> Construct a new Config Model object.

---

### `void loadEnv(const std::string& path)`

> Load environment variables from a file.

| Parameter | Description |
| --- | --- |
| `path` | The path to the .env file. |

---

### `QString getSmtpServer() const`

> Get the SMTP Server address.

**Returns:** QString The SMTP server hostname.

---

### `int getSmtpPort() const`

> Get the SMTP Port.

**Returns:** int The SMTP port number (default: 587).

---

### `QString getSmtpUsername() const`

> Get the SMTP Username.

**Returns:** QString The username for SMTP authentication.

---

### `QString getSmtpPassword() const`

> Get the SMTP Password.

**Returns:** QString The password for SMTP authentication.

---

### `QString getSmtpFrom() const`

> Get the Sender Email Address.

**Returns:** QString The email address to use in the 'From' field.

---

### `bool getSmtpStartTls() const`

> Check if STARTTLS is enabled.

**Returns:** true if STARTTLS is enabled (default), false otherwise.

---

### `QString getWatchDir() const`

> Get the Directory path to watch for files.

**Returns:** QString The absolute or relative path to the watch directory.

---

