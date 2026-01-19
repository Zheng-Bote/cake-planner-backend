<!-- DOCTOC SKIP -->
# Class Diagrams

This document visualizes the class structure and relationships of the Cake Planner Backend.

## Overview

The application follows a defined **Model-View-Controller (MVC)** pattern (where "View" is JSON serialization).
- **Controllers** handle HTTP requests.
- **Services** encapsulate business logic and external communications (e.g., SMTP).
- **Models** represent data structures and contain database interaction logic.

## Class Diagram

```mermaid
classDiagram
    %% Relationships
    AuthController ..> NotificationService : uses
    UserController ..> NotificationService : uses
    EventController ..> NotificationService : uses
    AdminController ..> NotificationService : uses

    NotificationService --> SmtpService : uses
    NotificationService ..> User : uses

    Event "1" --> "*" GalleryItem : contains
    Event "1" --> "1" EventRating : contains

    %% Controllers
    class AuthController {
        -NotificationService* m_notifyService
        +registerRoutes(App)
        +login()
        +register()
    }

    class UserController {
        +registerRoutes(App, NotificationService)
        +updateProfile()
        +getProfile()
    }

    class EventController {
        +registerRoutes(App, NotificationService)
        +createEvent()
        +listEvents()
        +rateEvent()
        +uploadPhoto()
    }

    class AdminController {
        +registerRoutes(App, NotificationService)
        +listUsers()
        +manageGroups()
    }

    class SystemController {
        +registerRoutes(App)
        +healthCheck()
        +sysInfo()
    }

    %% Services
    class NotificationService {
        -SmtpService* m_smtpService
        +sendWelcomeEmail(User)
        +sendPasswordReset(User)
    }

    class SmtpService {
        +sendMail(to, subject, body)
    }

    %% Models
    class User {
        +QString id
        +QString full_name
        +QString email
        +QString password_hash
        +QString groupId
        +bool is_admin
        +toJson()
        +getById(id)
        +getByEmail(email)
        +create()
    }

    class Event {
        +QString id
        +QString bakerId
        +QString groupId
        +QString date
        +vector~GalleryItem~ gallery
        +EventRating rating
        +toJson()
        +create()
        +getRange()
        +rateEvent()
    }

    class GalleryItem {
        +QString userId
        +QString photoUrl
    }

    class EventRating {
        +double average
        +int count
    }

    class ConfigModel {
        +getSmtpServer()
        +getWatchDir()
        +loadEnv()
    }
```
