# config_model.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Configuration Model implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `config_model.cpp` |
| **Description** | Configuration Model implementation |
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

### `ConfigModel::ConfigModel() = default`

> Constructs the ConfigModel.

---

### `void ConfigModel::loadEnv(const std::string& path) `

> Load environment variables from a file.

| Parameter | Description |
| --- | --- |
| `path` | The path to the .env file. |

---

### `QString ConfigModel::getSmtpServer() const `

> Gets the SMTP server address.

**Returns:** The SMTP server.

---

### `QString ConfigModel::getSmtpUsername() const `

> Gets the SMTP username.

**Returns:** The SMTP username.

---

### `QString ConfigModel::getSmtpPassword() const `

> Gets the SMTP password.

**Returns:** The SMTP password.

---

### `QString ConfigModel::getSmtpFrom() const `

> Gets the SMTP Sender email address.

**Returns:** The SMTP from address.

---

### `int ConfigModel::getSmtpPort() const `

> Gets the SMTP port.

**Returns:** The SMTP port.

---

### `bool ConfigModel::getSmtpStartTls() const `

> Checks if STARTTLS is enabled for SMTP.

**Returns:** True if STARTTLS is enabled, false otherwise.

---

### `QString ConfigModel::getWatchDir() const `

> Gets the directory being watched for changes.

**Returns:** The watch directory.

---

