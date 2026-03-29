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
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`struct EventRating `](#struct-eventrating-)
  - [`struct Event `](#struct-event-)
  - [`bool create(const QString& userId)`](#bool-createconst-qstring-userid)
  - [`static std::vector<Event> getRange(const QString &start, const QString &end, const QString &userId)`](#static-stdvectorevent-getrangeconst-qstring-start-const-qstring-end-const-qstring-userid)
  - [`static std::optional<Event> getById(const QString& eventId, const QString& currentUserId)`](#static-stdoptionalevent-getbyidconst-qstring-eventid-const-qstring-currentuserid)
  - [`static bool deleteEvent(const QString& eventId, const QString& currentUserId)`](#static-bool-deleteeventconst-qstring-eventid-const-qstring-currentuserid)
  - [`static bool rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment)`](#static-bool-rateeventconst-qstring-eventid-const-qstring-userid-int-stars-const-qstring-comment)
  - [`static std::vector<Event> getRanked(const QString& userId, int limit = 50)`](#static-stdvectorevent-getrankedconst-qstring-userid-int-limit--50)
  - [`std::string toIcsString() const`](#stdstring-toicsstring-const)

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

