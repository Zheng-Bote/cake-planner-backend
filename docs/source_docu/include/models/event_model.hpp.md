# event_model.hpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Unified Event Model |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `event_model.hpp` |
| **Description** | Unified Event Model |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `struct EventRating `

> EventRating struct.

---

### `struct Event `

> Event struct.

---

### `bool create(const QString& userId)`

> Function implementation.

---

### `static std::vector<Event> getRange(const QString &start, const QString &end, const QString &userId)`

> Function implementation.

---

### `static std::optional<Event> getById(const QString& eventId, const QString& currentUserId)`

> Function implementation.

---

### `static bool deleteEvent(const QString& eventId, const QString& currentUserId)`

> Function implementation.

---

### `static bool rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment)`

> Function implementation.

---

### `static std::vector<Event> getRanked(const QString& userId, int limit = 50)`

> Function implementation.

---

### `std::string toIcsString() const`

> Function implementation.

---

