# database.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | No description provided |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `database.cpp` |
| **Description** | No description provided |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `DatabaseManager &DatabaseManager::instance() `

> Gets the singleton instance of the DatabaseManager.

**Returns:** Reference to the singleton instance.

---

### `void DatabaseManager::initialize(const QString &path) `

> Initializes the database manager.

| Parameter | Description |
| --- | --- |
| `path` | The file path to the SQLite database. |

---

### `QSqlDatabase DatabaseManager::getDatabase() `

> Gets a thread-safe database connection.

**Returns:** A QSqlDatabase connection object.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `DatabaseManager::~DatabaseManager() `

> Destructor.

---

### `bool DatabaseManager::migrate() `

> Migrates the database schema.

**Returns:** True if migration succeeded, false otherwise.

---

### `CREATE INDEX IF NOT EXISTS idx_ratings_event_id ON ratings(event_id)`

> Function implementation.

---

### `CREATE INDEX IF NOT EXISTS idx_event_photos_event_id ON event_photos(event_id)`

> Function implementation.

---

### `CREATE INDEX IF NOT EXISTS idx_events_group_date ON events(group_id, event_date)`

> Function implementation.

---

### `QSqlQuery query(db)`

> Function implementation.

---

### `QSqlQuery checkQ(db)`

> Function implementation.

---

