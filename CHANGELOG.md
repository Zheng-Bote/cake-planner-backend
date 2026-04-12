<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Changelog](#changelog)
  - [[1.2.0] - 2026-04-11](#120---2026-04-11)
    - [Added](#added)
    - [Changed](#changed)
  - [[1.1.0] - 2026-03-29](#110---2026-03-29)
    - [Added](#added-1)
    - [Changed](#changed-1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.4.0] - 2026-04-12

### Added
- **User Preference**: Added `receive_event_emails` option in user settings, allowing users to opt-out of automated cake event notifications.
- **AI Configuration**: The AI Translation API URL is now configurable via the `AI_TXT_API_URL` environment variable.
- **Email Stability**: Introduced a connection queue in `SmtpService` to limit concurrent SMTP connections, preventing resource exhaustion during large group mail sends.
- **Translation Protection**: Added a 10-second timeout for AI translation requests to prevent server hangs.
- **Enhanced Logging**: Integrated comprehensive `spdlog` tracking for group mail dispatch and translation status.

### Changed
- **Database Schema**: Updated `users` table to include `receive_event_emails` column (Default: 1).
- **API Endpoints**: 
    - `POST /api/user/settings` now supports `receiveEventEmails`.
    - `POST /api/register` now supports optional `receiveEventEmails`.
- **Notification Logic**: Event notifications (creation and deletion) now respect the user's opt-out preference.
- **JWT Implementation**: Updated `jwt-cpp` integration to version 0.7.1, now utilizing `nlohmann_json` traits. Moved to `FetchContent` to ensure compatibility with system OpenSSL.
- **Refactoring**: Refactored `ConfigModel` to use the project's internal `EnvLoader`, allowing for the removal of the redundant `dotenv-cpp` library.

## [1.3.0] - 2026-04-12

### Added
- **Package Management**: Migrated the build system to use **Conan 2** for managing external dependencies (Qt6, Crow, Asio, spdlog, etc.).
- **Build Automation**: Added `conan_install.sh` to automate Conan profile detection and dependency installation.
- **Docker Enhancement**: Updated `Dockerfile` to include Conan-based multi-stage building.

### Changed
- **Build System**: Significant cleanup of `CMakeLists.txt`, removing most `FetchContent` blocks in favor of Conan packages.
- **Dependency Purge**: Removed unused `dotenv-cpp` library.

## [1.2.0] - 2026-04-11

### Added

-   **AI-Powered Group Emails**: Administrators (Global and Group Admins) can now send messages to entire groups with automatic translation into each recipient's preferred email language.
-   **Translation API Integration**: Integrated with a local LLM/Translation API (`/api/v1/prompt`) to handle multi-language message delivery.
-   **Admin Documentation**: New documentation at `docs/admin_guide.md` describing the group email feature.

### Changed

-   **Build System**: Added `Qt6::Network` dependency to support communication with external translation services.
-   **Notification Service**: Extended `NotificationService` to support group-wide messaging and synchronized translation workflows.

## [1.1.0] - 2026-03-29

### Added

-   **Calendar Integration**: New event email notifications now include a `.ics` calendar attachment for easy scheduling.
-   **SMTP Service Expansion**: Enhanced `SmtpService` to support file attachments in asynchronous email delivery.
-   **ICS Model Enhancements**: Improved `toIcsString()` in `EventModel` to support full-day events with proper start and end dates.

### Changed

-   **Notification Service**: Updated `notifyGroupNewEvent` to handle ICS data payloads for group notifications.
-   **Event Creation Flow**: The backend now automatically generates and attaches calendar files when a user creates a new event.
