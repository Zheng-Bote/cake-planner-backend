# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-03-29

### Added

-   **Calendar Integration**: New event email notifications now include a `.ics` calendar attachment for easy scheduling.
-   **SMTP Service Expansion**: Enhanced `SmtpService` to support file attachments in asynchronous email delivery.
-   **ICS Model Enhancements**: Improved `toIcsString()` in `EventModel` to support full-day events with proper start and end dates.

### Changed

-   **Notification Service**: Updated `notifyGroupNewEvent` to handle ICS data payloads for group notifications.
-   **Event Creation Flow**: The backend now automatically generates and attaches calendar files when a user creates a new event.
