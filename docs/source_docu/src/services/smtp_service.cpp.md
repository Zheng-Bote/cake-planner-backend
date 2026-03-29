# smtp_service.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | SMTP Service Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `smtp_service.cpp` |
| **Description** | SMTP Service Implementation |
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

### `namespace service `

> service namespace.

---

### `SmtpService::SmtpService(const model::ConfigModel& config, QObject* parent)`

> Constructs the SmtpService.

| Parameter | Description |
| --- | --- |
| `config` | Configuration model containing SMTP settings. |
| `parent` | Optional parent QObject. |

---

### `void SmtpService::sendEmailAsync(const QString& to, const QString& subject, const QString& body,`

> Sends an email asynchronously using Qt's event loop.

| Parameter | Description |
| --- | --- |
| `to` | Recipient email address. |
| `subject` | Email subject. |
| `body` | Email body content. |
| `attachmentData` | Optional attachment data. |
| `attachmentName` | Optional attachment filename. |

---

### `void SmtpService::doSendEmail(const QString& to, const QString& subject, const QString& body,`

> Internal slot to perform the actual email sending.

| Parameter | Description |
| --- | --- |
| `to` | Recipient email address. |
| `subject` | Email subject. |
| `body` | Email body content. |
| `attachmentData` | Optional attachment data. |
| `attachmentName` | Optional attachment filename. |

---

