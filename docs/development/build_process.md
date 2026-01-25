# Build Process & Development Guide

This guide explains how to build the Cake Planner Backend from source and how to package it as an AppImage.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Build Process \& Development Guide](#build-process--development-guide)
  - [Prerequisites](#prerequisites)
    - [System Requirements](#system-requirements)
    - [Installing Dependencies (Ubuntu/Debian)](#installing-dependencies-ubuntudebian)
  - [Building from Source](#building-from-source)
  - [creating an AppImage](#creating-an-appimage)
    - [Script: `create_appimage.sh`](#script-create_appimagesh)
    - [Usage](#usage)
    - [Result](#result)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Prerequisites

The project requires a modern Linux environment with C++23 support.

### System Requirements

- **Linux**: (Ubuntu 24.04 recommended)
- **CMake**: 3.23+
- **Compiler**: GCC 13+ or Clang 16+ (Must support **C++23**).
- **Build System**: CMake 3.24 or newer.
- **Dependencies**:
  - **Qt6**: Modules `Core`, `Sql`, `Gui`.
  - **OpenSSL**: `libssl-dev`.
  - **Git**, **Make**, **wget** (for fetching resources).

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install qt6-base-dev libqt6sql6-sqlite libssl-dev
```

## Building from Source

The project uses CMake as its build system.

1. **Clone the Repository**:

```bash
    git clone https://github.com/Zheng-Bote/cake-planner-backend.git
    cd cake-planner-backend
```

2. **Create a Build Directory**:

```bash
mkdir build
cd build
```

1. **Configure the Project**:

```bash
cmake ..
```

*Note: Dependencies like Crow, spdlog, etc., are automatically fetched via CMake `FetchContent`.*

3. **Compile**:

```bash
make -j$(nproc)
```

4. **Run**:

```bash
./CakePlanner
```

## creating an AppImage

The project includes a script to package the application as a standalone **AppImage**.

### Script: `create_appimage.sh`

This script automates the following steps:

1. Rebuilds the project in `Release` mode.
2. Creates an `AppDir` structure.
3. Copies binaries, icons, and `.desktop` files.
4. Downloads `linuxdeploy` and the `qt-plugin`.
5. Generates the final `.AppImage` file.

### Usage

Run the script from the project root:

```bash
chmod +x create_appimage.sh
./create_appimage.sh
```

### Result

After successful execution, you will find `CakePlanner-x86_64.AppImage` in the root directory. You can run it directly:

```bash
./CakePlanner-x86_64.AppImage
```

> **Note**: AppImages require FUSE to run. On newer Ubuntu versions (22.04+), you might need to install `libfuse2`:
> `sudo apt install libfuse2`
