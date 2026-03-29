# event_controller.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Event Controller Implementation (Safe Blocking Long Polling) |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `event_controller.cpp` |
| **Description** | Event Controller Implementation (Safe Blocking Long Polling) |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`struct LatestEventData `](#struct-latesteventdata-)
  - [`void broadcastNewEvent(const Event& evt) `](#void-broadcastneweventconst-event-evt-)
  - [`std::lock_guard<std::mutex> lock(event_mutex)`](#stdlock_guardstdmutex-lockevent_mutex)
  - [`namespace rz `](#namespace-rz-)
  - [`namespace controller `](#namespace-controller-)
  - [`void EventController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) `](#void-eventcontrollerregisterroutescrowapprzmiddlewareauthmiddleware-app-servicenotificationservice-notifyservice-)
  - [`std::lock_guard<std::mutex> lock(event_mutex)`](#stdlock_guardstdmutex-lockevent_mutex-1)
  - [`std::unique_lock<std::mutex> lock(event_mutex)`](#stdunique_lockstdmutex-lockevent_mutex)
  - [`crow::multipart::message msg(req)`](#crowmultipartmessage-msgreq)
  - [`QFile file(fullPath)`](#qfile-filefullpath)
  - [`crow::multipart::message msg(req)`](#crowmultipartmessage-msgreq-1)
  - [`QFile file(fullPath)`](#qfile-filefullpath-1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `struct LatestEventData `

> LatestEventData struct.

---

### `void broadcastNewEvent(const Event& evt) `

> Thread-safe helper to broadcast a new event to all waiting SSE clients.

| Parameter | Description |
| --- | --- |
| `evt` | The event to broadcast. |

---

### `std::lock_guard<std::mutex> lock(event_mutex)`

> Function implementation.

---

### `namespace rz `

> rz namespace.

---

### `namespace controller `

> controller namespace.

---

### `void EventController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) `

> Registers event-related routes with the Crow application.

| Parameter | Description |
| --- | --- |
| `app` | The Crow application instance to register routes with. |
| `notifyService` | Pointer to the notification service for email notifications. |

---

### `std::lock_guard<std::mutex> lock(event_mutex)`

> Function implementation.

---

### `std::unique_lock<std::mutex> lock(event_mutex)`

> Function implementation.

---

### `crow::multipart::message msg(req)`

> Function implementation.

---

### `QFile file(fullPath)`

> Function implementation.

---

### `crow::multipart::message msg(req)`

> Function implementation.

---

### `QFile file(fullPath)`

> Function implementation.

---

