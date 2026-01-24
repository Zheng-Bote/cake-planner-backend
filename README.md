# Cake Planner Backend

A high-performance C++ REST API backend for the Cake Planner application, built with the [Crow](https://crowcpp.org/) microframework and [Qt6](https://www.qt.io/).

[Report Issue](https://github.com/Zheng-Bote/cake-planner-backend/issues) · [Request Feature](https://github.com/Zheng-Bote/cake-planner-backend/pulls)

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Zheng-Bote/cake-planner-backend?logo=GitHub)](https://github.com/Zheng-Bote/cake-planner-backend/releases)

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [🍰 Overview](#-overview)
- [🚀 Key Features](#-key-features)
- [🛠️ Technology Stack](#-technology-stack)
- [📚 Documentation](#-documentation)
- [📦 Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Quick Build](#quick-build)
- [📄 License](#-license)
- [🤝 Authors](#-authors)
  - [Code Contributors](#code-contributors)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## 🍰 Overview

![Qt](https://img.shields.io/badge/Qt-6.0%2B-green.svg)
![Language](https://img.shields.io/badge/Language-C%2B%2B23-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)


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

Detailed documentation is available on the [Github Pages](https://zheng-bote.github.io/cake-planner-backend/)

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

Copyright (c) 2026 ZHENG Robert

## 🤝 Authors

- [![Zheng Robert - Core Development](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

### Code Contributors

![Contributors](https://img.shields.io/github/contributors/Zheng-Bote/cake-planner-backend?color=dark-green)

[![Zheng Robert](https://img.shields.io/badge/Github-Zheng_Robert-black?logo=github)](https://www.github.com/Zheng-Bote)

---

:vulcan_salute:
