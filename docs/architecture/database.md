<!-- DOCTOC SKIP -->

# Database Schema

The Cake Planner Backend uses **SQLite** as its relational database. The database file is typically located at `data/cakeplanner.sqlite`.

## Entity-Relationship Diagram (ERD)

The following diagram shows the database tables and their relationships.

```mermaid
erDiagram
    USERS ||--o{ GROUP_MEMBERS : "is member of"
    GROUPS ||--o{ GROUP_MEMBERS : "has members"
    GROUPS ||--o{ EVENTS : "hosts"
    USERS ||--o{ EVENTS : "bakes for (baker_id)"
    EVENTS ||--o{ RATINGS : "has"
    USERS ||--o{ RATINGS : "creates"
    EVENTS ||--o{ EVENT_PHOTOS : "contains"
    USERS ||--o{ EVENT_PHOTOS : "uploads"

    USERS {
        TEXT id PK
        TEXT full_name
        TEXT email uk
        TEXT password_hash
        TEXT language "default 'en'"
        TEXT email_language "default 'en'"
        TEXT totp_secret
        INTEGER is_active "0 or 1"
        INTEGER is_admin "0 or 1"
        INTEGER must_change_password "0 or 1"
        TEXT created_at
        TEXT updated_at
    }

    GROUPS {
        TEXT id PK
        TEXT name
        TEXT description
        TEXT created_at
    }

    GROUP_MEMBERS {
        TEXT user_id PK, FK
        TEXT group_id PK, FK
        TEXT role "member, etc."
        TEXT joined_at
    }

    EVENTS {
        TEXT id PK
        TEXT group_id FK
        TEXT baker_id FK
        TEXT event_date
        TEXT description
        TEXT photo_path
        TEXT created_at
    }

    RATINGS {
        TEXT id PK
        TEXT event_id FK
        TEXT rater_id FK
        INTEGER rating_value "1-5"
        TEXT comment
        TEXT created_at
    }

    EVENT_PHOTOS {
        TEXT event_id PK, FK
        TEXT user_id PK, FK
        TEXT photo_path
        TEXT uploaded_at
    }
```

## Table Definitions

### `users`
Stores user account information.
- **Primary Key**: `id` (UUID as Text)
- **Unique**: `email`

### `groups`
Represents groups (e.g., departments, teams) that organize cake events.
- **Primary Key**: `id`

### `group_members`
Many-to-Many link between `users` and `groups`.
- **Composite Primary Key**: (`user_id`, `group_id`)
- **Foreign Keys**: Cascade delete on both user and group.

### `events`
Represents a cake bringing event on a specific date.
- **Primary Key**: `id`
- **Foreign Keys**:
  - `group_id`: The group this event belongs to.
  - `baker_id`: The user responsible for baking.

### `ratings`
Allows users to rate and comment on events.
- **Unique Constraint**: A user can only rate an event once (`event_id`, `rater_id`).
- **Check Constraint**: `rating_value` must be between 1 and 5.

### `event_photos`
Additional photos uploaded to an event gallery.
- **Composite Primary Key**: (`event_id`, `user_id`) - *Note: Schema implies one photo per user per event based on PK, or just efficient linking.*

## Migration System

Database migrations are handled automatically in `DatabaseManager::migrate()`.
- On startup, the application checks if tables exist.
- If not, it executes `CREATE TABLE IF NOT EXISTS` statements.
- Uses `PRAGMA foreign_keys = ON` to enforce integrity.
