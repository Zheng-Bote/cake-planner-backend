# Database Schema

The Cake Planner Backend uses **SQLite** as its relational database. The database file is typically located at `data/cakeplanner.sqlite`.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Description](#description)
- [Database Schema (overview)](#database-schema-overview)
- [Entity Relationship Diagram (ERD)](#entity-relationship-diagram-erd)
- [Tables](#tables)
  - [users](#users)
  - [groups](#groups)
  - [group_members](#group_members)
  - [events](#events)
  - [ratings](#ratings)
  - [event_photos](#event_photos)
- [Migration System](#migration-system)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Description

This document describes the database schema for the **Cake Planner Backend**. The application uses an **SQLite** database.

## Database Schema (overview)

The database is normalized to ensure data integrity.

- users: Stores credentials and profile data.
- groups: Manages baking circles.
- group_members: Links users to groups (n:m).
- events: The core entity (Date, Baker, Description).
- ratings: Links users to events with a score (1-5).
- event_photos: Stores paths to community uploads linked to events.

## Entity Relationship Diagram (ERD)

![ERD](../../assets/img/architecture/backend/db_erp.png)

## Tables

### users

Stores user account information, including authentication details and preferences.

| Column | Type | Description |
| :--- | :--- | :--- |
| `id` | `TEXT` | Primary Key (UUID). |
| `full_name` | `TEXT` | Full name of the user. |
| `email` | `TEXT` | Unique email address. |
| `password_hash` | `TEXT` | Argon2id password hash. |
| `language` | `TEXT` | Preferred UI language (e.g., 'en', 'de'). |
| `email_language` | `TEXT` | Preferred language for emails. |
| `totp_secret` | `TEXT` | Secret for 2FA (if enabled). |
| `is_active` | `INTEGER` | 1 if active, 0 if inactive. |
| `is_admin` | `INTEGER` | 1 if global admin, 0 otherwise. |
| `must_change_password`| `INTEGER` | 1 if user must change password next login. |
| `temp_password_hash` | `TEXT` | Hash of a temporary password (forgot password flow). |
| `temp_password_expiry`| `TEXT` | Expiration timestamp of the temp password. |
| `last_login_at` | `TEXT` | Timestamp of last login. |
| `created_at` | `TEXT` | Creation timestamp. |
| `updated_at` | `TEXT` | Update timestamp. |

### groups

Represents groups (departments, teams) that can organize cake events.

| Column | Type | Description |
| :--- | :--- | :--- |
| `id` | `TEXT` | Primary Key (UUID). |
| `name` | `TEXT` | Name of the group. |
| `description` | `TEXT` | Optional description. |
| `created_at` | `TEXT` | Creation timestamp. |

### group_members

Link table defining the many-to-many relationship between users and groups.

| Column | Type | Description |
| :--- | :--- | :--- |
| `user_id` | `TEXT` | Foreign Key to `users.id`. |
| `group_id` | `TEXT` | Foreign Key to `groups.id`. |
| `role` | `TEXT` | Role in group (e.g., 'member', 'admin'). |
| `joined_at` | `TEXT` | Timestamp when user joined. |

**Primary Key:** Composite (`user_id`, `group_id`)

### events

Represents a "Cake Event" where a user brings a cake on a specific date.

| Column | Type | Description |
| :--- | :--- | :--- |
| `id` | `TEXT` | Primary Key (UUID). |
| `group_id` | `TEXT` | The group this event belongs to. |
| `baker_id` | `TEXT` | The user bringing the cake. |
| `event_date` | `TEXT` | Date of the event (`YYYY-MM-DD`). |
| `description` | `TEXT` | Description of the cake/event. |
| `photo_path` | `TEXT` | Path to the cover image. |
| `created_at` | `TEXT` | Creation timestamp. |

### ratings

Allows users to rate and comment on events.

| Column | Type | Description |
| :--- | :--- | :--- |
| `id` | `TEXT` | Primary Key (UUID). |
| `event_id` | `TEXT` | Foreign Key to `events.id`. |
| `rater_id` | `TEXT` | Foreign Key to `users.id` (the rater). |
| `rating_value` | `INTEGER`| 1 to 5 stars. |
| `comment` | `TEXT` | Optional text comment. |
| `created_at` | `TEXT` | Creation timestamp. |

**Constraints:** A user can rate each event only once (UNIQUE `event_id`, `rater_id`).

### event_photos

Stores additional photos uploaded to the gallery of an event.

| Column | Type | Description |
| :--- | :--- | :--- |
| `event_id` | `TEXT` | Foreign Key to `events.id`. |
| `user_id` | `TEXT` | Foreign Key to `users.id` (uploader). |
| `photo_path` | `TEXT` | Path to the uploaded photo. |
| `uploaded_at` | `TEXT` | Upload timestamp. |

**Primary Key:** Composite (`event_id`, `user_id`). Currently, users can upload only one photo per event gallery (excluding the cover photo if they are the baker, which is stored in `events`).

## Migration System

Database migrations are handled automatically in `DatabaseManager::migrate()`.
- On startup, the application checks if tables exist.
- If not, it executes `CREATE TABLE IF NOT EXISTS` statements.
- Uses `PRAGMA foreign_keys = ON` to enforce integrity.

