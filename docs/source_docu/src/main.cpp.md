# main.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Entry Point |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `main.cpp` |
| **Description** | Entry Point |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`namespace fs = std::filesystem`](#namespace-fs--stdfilesystem)
  - [`void signalHandler(int signum) `](#void-signalhandlerint-signum-)
  - [`int main(int argc, char *argv[]) `](#int-mainint-argc-char-argv-)
  - [`QCoreApplication qtApp(argc, argv)`](#qcoreapplication-qtappargc-argv)
  - [`else spdlog::set_level(spdlog::level::info)`](#else-spdlogset_levelspdloglevelinfo)
  - [`std::filesystem::path dbP(dbFullPath)`](#stdfilesystempath-dbpdbfullpath)
  - [`rz::service::SmtpService smtpService(configModel, &qtApp)`](#rzservicesmtpservice-smtpserviceconfigmodel-qtapp)
  - [`rz::service::NotificationService notifyService(&smtpService)`](#rzservicenotificationservice-notifyservicesmtpservice)
  - [`rz::controller::AuthController authController(&notifyService)`](#rzcontrollerauthcontroller-authcontrollernotifyservice)
  - [`std::thread serverThread([&app, serverPort, &qtApp]()`](#stdthread-serverthreadapp-serverport-qtapp)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `namespace fs = std::filesystem`

> fs namespace.

---

### `void signalHandler(int signum) `

> Signal handler callback.

| Parameter | Description |
| --- | --- |
| `signum` | The signal number (e.g., SIGINT, SIGTERM). |

---

### `int main(int argc, char *argv[]) `

> Main entry point of the application.

| Parameter | Description |
| --- | --- |
| `argc` | Argument count. |
| `argv` | Argument vector. |

**Returns:** Exit code.

---

### `QCoreApplication qtApp(argc, argv)`

> Function implementation.

---

### `else spdlog::set_level(spdlog::level::info)`

> Function implementation.

---

### `std::filesystem::path dbP(dbFullPath)`

> Function implementation.

---

### `rz::service::SmtpService smtpService(configModel, &qtApp)`

> Function implementation.

---

### `rz::service::NotificationService notifyService(&smtpService)`

> Function implementation.

---

### `rz::controller::AuthController authController(&notifyService)`

> Function implementation.

---

### `std::thread serverThread([&app, serverPort, &qtApp]()`

> Function implementation.

---

