# Cake Planner Backend

A high-performance C++ REST API backend for the Cake Planner application, built with the [Crow](https://crowcpp.org/) microframework and [Qt6](https://www.qt.io/).

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

  - [🌟 What is CakePlanner?](#-what-is-cakeplanner)
    - [✨ Key Features](#-key-features)
  - [🚀 How it works](#-how-it-works)
  - [🖼️ Screenshots](#-screenshots)
  - [🛠️ For Administrators (The Technical Stuff)](#-for-administrators-the-technical-stuff)
    - [Admin-Panel](#admin-panel)
      - [Screenshots](#screenshots)
    - [Quick Start](#quick-start)
- [See more](#see-more)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

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

## 📚 Documentation

Detailed documentation is available in the `docs/` directory:

-   **[Architecture](docs/architecture/crow_server.md)**: Overview of the server components, controllers, and request flow.
-   **[Class Diagrams](docs/architecture/class_diagrams.md)**: Visual representation of classes and relationships.
-   **[Database](docs/architecture/database.md)**: Entity-Relationship Diagram (ERD) and Schema definitions.
-   **[Security](docs/security/backend_security.md)**: In-depth look at authentication, encryption, and data protection measures.
-   **[Development](docs/development/build_process.md)**: Guide on how to build the project from source and create AppImages..
-   **[Deployment](docs/deployment/deployment_guide.md)**: Instructions for deploying using Docker Compose and Nginx.

## 📦 Getting Started

### Prerequisites
-   Linux (Ubuntu 24.04 recommended)
-   CMake 3.24+
-   Qt6 & OpenSSL

### Quick Build
```bash
git clone https://github.com/Zheng-Bote/cake-planner-backend
cd cake-planner-backend
mkdir build && cd build
cmake ..
make -j$(nproc)
./CakePlanner
```

For full build instructions, including AppImage creation, see the [Build Guide](docs/development/build_process.md).

## 📄 License
This project is licensed under the **MIT License**.
