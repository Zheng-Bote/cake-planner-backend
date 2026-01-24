# Backend Security

This document outlines the security measures implemented in the Cake Planner Backend to protect user data, ensure integrity, and prevent common vulnerabilities.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [1. Authentication & Authorization](#1-authentication--authorization)
  - [JSON Web Tokens (JWT)](#json-web-tokens-jwt)
  - [Role-Based Access Control (RBAC)](#role-based-access-control-rbac)
- [2. Password Security](#2-password-security)
  - [Hashing Algorithm: Argon2id](#hashing-algorithm-argon2id)
- [3. Data Protection](#3-data-protection)
  - [SQL Injection Prevention](#sql-injection-prevention)
  - [Input Validation](#input-validation)
- [4. Transport Security](#4-transport-security)
  - [HTTPS / TLS](#https--tls)
  - [CORS (Cross-Origin Resource Sharing)](#cors-cross-origin-resource-sharing)
- [5. Security Best Practices](#5-security-best-practices)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## 1. Authentication & Authorization

### JSON Web Tokens (JWT)
The application uses **JWT (JSON Web Tokens)** for stateless authentication.
- **Library**: `token_utils.hpp` handles token generation and verification.
- **Structure**: Tokens contain a payload with the User ID, Email, and Roles (Admin/User).
- **Transport**: Tokens must be sent in the `Authorization` header: `Authorization: Bearer <TOKEN>`.
- **Validation**:
    - The `AuthMiddleware` verifies the token signature on every protected request.
    - Expired or tampered tokens result in `401 Unauthorized` or `403 Forbidden`.

### Role-Based Access Control (RBAC)
- **Middleware**: `AuthMiddleware` injects the current user context into the request.
- **Controllers**: Individual controllers (e.g., `AdminController`) check `user.is_admin` to restrict access to sensitive endpoints.

## 2. Password Security

User passwords are **never** stored in plain text.

### Hashing Algorithm: Argon2id
The application uses **Argon2id**, the winner of the Password Hashing Competition (PHC) and currently recommended by OWASP.

- **Implementation**: `src/utils/password_utils.cpp`
- **Parameters**:
    - **T-Cost (Iterations)**: 3
    - **M-Cost (Memory)**: 64 MB (Hardens against GPU/ASIC attacks)
    - **Parallelism**: 4 Threads
    - **Salt**: 16 Bytes random salt generated per password.
    - **Hash Length**: 32 Bytes.

## 3. Data Protection

### SQL Injection Prevention
All database interactions via `DatabaseManager` and Models use **Prepared Statements** (via `QSqlQuery`).
- **Mechanism**: Inputs are bound using `bindValue()` (e.g., `:email`, `:id`) instead of string concatenation.
- **Benefit**: This effectively neutralizes SQL injection attacks.

### Input Validation
- **Strong Typing**: The use of C++ struct models (`User`, `Event`) ensures data types are respected.
- **Crow Routing**: Route parameters are typed (e.g., `<string>`, `<int>`), preventing basic malformed URL attacks.

## 4. Transport Security

### HTTPS / TLS
The Crow server runs on **HTTP** internally (default port 8080).
- **Recommendation**: For production, **ALWAYS** sit the application behind a Reverse Proxy (Nginx, Apache, Traefik).
- **SSL Termination**: The reverse proxy should handle SSL/TLS encryption (HTTPS) and forward requests to the backend via localhost.

### CORS (Cross-Origin Resource Sharing)
- The application implements CORS handling in middleware to allow requests from the frontend application.
- **Preflight Checks**: `OPTIONS` requests are handled globally to support browser preflight checks.

## 5. Security Best Practices
- **2FA (Two-Factor Authentication)**: The backend supports TOTP (Time-based One-Time Password) for an additional layer of login security (`totp_utils.hpp`).
- **Rate Limiting**: It is recommended to configure Rate Limiting on the **Reverse Proxy** level (e.g., `limit_req` in Nginx) to prevent Brute Force or DDoS attacks.
- **Environment Variables**: Sensitive secrets (Database path, API keys, SMTP credentials) are loaded from `.env` and **not** hardcoded.
