<!-- DOCTOC SKIP -->
# Cake Planner Backend

A high-performance C++ REST API backend for the Cake Planner application, built with the [Crow](https://crowcpp.org/) microframework and [Qt6](https://www.qt.io/).

## 🍰 Overview
The **Cake Planner Backend** serves as the core infrastructure for coordinating cake-baking events. It manages users, departments (groups), and schedules to ensure there is always cake in the office!

The application is designed to be lightweight yet robust, leveraging C++23 for performance and Qt6 for reliable cross-platform core functionalities.

## 🚀 Key Features

-   **High Performance API**: Asynchronous REST API powered by Crow.
-   **Security First**:
    -   Stateless Authentication using **JWT** (JSON Web Tokens).
    -   Secure Password Storage using **Argon2id**.
    -   Protection against SQL Injection via **Qt SQL** prepared statements.
-   **Structured Data**: Manages Users, Groups, Events, Ratings, and Photo Galleries.
-   **Easy Deployment**:
    -   Compiles into a single standalone **AppImage**.
    -   Docker-ready with **Docker Compose** support.
-   **Integrated Services**: SMTP Email Notifications (Welcome, Password Reset) and Internal Health Checks.

## 🛠️ Technology Stack

| Component | Technology | Description |
| :--- | :--- | :--- |
| **Language** | C++ 23 | Modern C++ features. |
| **Web Framework** | Crow | Fast, microframework for C++. |
| **Core Libs** | Qt 6 (Core, Sql) | Event loop, String manipulation, Database/SQL abstraction. |
| **Database** | SQLite | Serverless, self-contained SQL database engine. |
| **Logging** | spdlog | Fast C++ logging library. |
| **JSON** | nlohmann/json | JSON serialization/deserialization. |
| **Security** | Argon2id / OpenSSL | Cryptography and hashing. |

## 📦 Getting Started

- For Setup, see the [Deployment](deployment/setup.html).
- For full build instructions, including AppImage creation, see the [Development](development/build_process.html).

## 📄 License
This project is licensed under the **MIT License**.
