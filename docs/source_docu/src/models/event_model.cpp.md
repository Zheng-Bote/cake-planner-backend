# event_model.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Event Model Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `event_model.cpp` |
| **Description** | Event Model Implementation |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `crow::json::wvalue Event::toJson() const `

> Converts the Event object to a JSON value.

**Returns:** A crow::json::wvalue containing the event data.

---

### `std::vector<Event> Event::getRange(const QString &start,`

> Retrieves a list of events within a specific date range for a user.

| Parameter | Description |
| --- | --- |
| `start` | The start date (inclusive, format yyyy-MM-dd). |
| `end` | The end date (inclusive, format yyyy-MM-dd). |
| `userId` | The ID of the user requesting the events. |

**Returns:** A vector of Event objects.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool Event::create(const QString &userId) `

> Creates a new event in the database.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the user creating the event (the baker). |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery userQuery(db)`

> Function implementation.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `std::optional<Event> Event::getById(const QString& eventId, const QString& currentUserId) `

> Retrieves a specific event by its ID.

| Parameter | Description |
| --- | --- |
| `eventId` | The ID of the event to retrieve. |
| `currentUserId` | The ID of the currently logged-in user. |

**Returns:** An optional Event object if found, std::nullopt otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool Event::deleteEvent(const QString& eventId, const QString& currentUserId) `

> Deletes an event.

| Parameter | Description |
| --- | --- |
| `eventId` | The ID of the event to delete. |
| `currentUserId` | The ID of the user attempting to delete the event. |

**Returns:** True if successful and authorized, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `bool Event::rateEvent(const QString& eventId, const QString& userId, int stars, const QString& comment) `

> Rates an event.

| Parameter | Description |
| --- | --- |
| `eventId` | The ID of the event to rate. |
| `userId` | The ID of the user submitting the rating. |
| `stars` | The number of stars (rating value). |
| `comment` | An optional comment. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `INSERT INTO ratings (id, event_id, rater_id, rating_value, comment)`

> Function implementation.

---

### `bool Event::uploadPhoto(const QString& eventId, const QString& userId, const QString& filename) `

> Uploads a photo to an event's gallery.

| Parameter | Description |
| --- | --- |
| `eventId` | The ID of the event. |
| `userId` | The ID of the user uploading the photo. |
| `filename` | The relative path/filename of the uploaded photo. |

**Returns:** True if successful, false otherwise.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `INSERT INTO event_photos (event_id, user_id, photo_path, uploaded_at)`

> Function implementation.

---

### `QSqlQuery updateCover(db)`

> Function implementation.

---

### `std::string Event::toIcsString() const `

> Generates an ICS (iCalendar) string for the event.

**Returns:** The ICS string.

---

### `std::vector<Event> Event::getRanked(const QString& userId, int limit) `

> Retrieves a list of events ranked by their average rating.

| Parameter | Description |
| --- | --- |
| `userId` | The ID of the requesting user (used for group filtering). |
| `limit` | The maximum number of events to return. |

**Returns:** A vector of ranked Event objects.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `QSqlQuery galQuery(db)`

> Function implementation.

---

