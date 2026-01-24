# Backend Class Design

The Backend is built using modern C++23 and the Crow framework. It follows a layered MVC-like architecture (Controller-Model-Service).

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [Class Diagrams](#class-diagrams)
  - [Overview](#overview)
  - [Class Diagram](#class-diagram)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Description

This document visualizes the class structure and relationships of the Cake Planner Backend.

## Class Diagrams

The following diagram illustrates the core classes and their relationships. To ensure readability, the classes are arranged in logical tiers with a maximum of four components per row.

![Class Diagram](../../assets/img/architecture/backend/crow_class-diagrams.png)

## Component Details

The application follows a defined **Model-View-Controller (MVC)** pattern (where "View" is JSON serialization).

### Controllers

Controllers handle incoming HTTP requests, parse JSON bodies, and delegate logic to Models or Services. They are the entry point for the REST API.

- **AuthController**: Handles Registration, Login, Password Reset, and 2FA setup.
- **UserController**: Manages user profiles, settings (language), and self-deletion.
- **EventController**: Handles event creation, listing, deletion, rating, and photo uploads.
- **AdminController**: Provides endpoints for user and group management (Admin only).
- **SystemController**: Exposes system health and version information.

### Models

Models represent the data entities and encompass the business logic and database interactions.

- **User**: Represents a registered user. Handles authentication checks, password hashing, and group membership logic.
- **Event**: Represents a cake event. Handles creation rules, date filtering, and rating calculations.
- **ConfigModel**: Loads and provides access to application configuration (env vars).

### Services

Services encapsulate business logic that spans multiple models or involves external communication.

- **NotificationService**: Abstraction layer for sending business-level emails (e.g., "New Cake Alert"). localized in German/English.
- **SmtpService**: Low-level wrapper around the SMTP library to send raw emails asynchronously via Qt's event loop.

### Middleware

- **AuthMiddleware**: Intercepts every request (except `login`/`register`) to validate the JWT Bearer Token. It populates the request context with the authenticated user's ID and role.

### Utilities & Core

Helper classes for common tasks.

- **DatabaseManager**: Singleton that manages the SQLite connection pool and handles schema migrations.
- **TokenUtils**: Generates and verifies JWTs (JSON Web Tokens).
- **PasswordUtils**: Handles secure password hashing using Argon2id.
- **ImageProcessor**: Resizes uploaded images and converts them to WebP format.
