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
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace rz `](#namespace-rz-)
  - [`namespace model `](#namespace-model-)
  - [`ConfigModel::ConfigModel() = default`](#configmodelconfigmodel--default)
  - [`void ConfigModel::loadEnv(const std::string& path) `](#void-configmodelloadenvconst-stdstring-path-)
  - [`QString ConfigModel::getSmtpServer() const `](#qstring-configmodelgetsmtpserver-const-)
  - [`QString ConfigModel::getSmtpUsername() const `](#qstring-configmodelgetsmtpusername-const-)
  - [`QString ConfigModel::getSmtpPassword() const `](#qstring-configmodelgetsmtppassword-const-)
  - [`QString ConfigModel::getSmtpFrom() const `](#qstring-configmodelgetsmtpfrom-const-)
  - [`int ConfigModel::getSmtpPort() const `](#int-configmodelgetsmtpport-const-)
  - [`bool ConfigModel::getSmtpStartTls() const `](#bool-configmodelgetsmtpstarttls-const-)
  - [`QString ConfigModel::getWatchDir() const `](#qstring-configmodelgetwatchdir-const-)

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

