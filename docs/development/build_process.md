# Build Process & Development Guide

This guide explains how to build the Cake Planner Backend from source and how to package it as an AppImage.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

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
- **Python 3**: To install and run Conan.
- **Conan**: 2.0 or newer.

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake git python3-pip
sudo apt install qt6-base-dev libqt6sql6-sqlite libssl-dev
pip install conan --break-system-packages
```

## Building from Source

The project uses CMake and **Conan 2** for dependency management.

1. **Clone the Repository**:

```bash
    git clone https://github.com/Zheng-Bote/cake-planner-backend.git
    cd cake-planner-backend
```

2. **Install Dependencies using Conan**:

```bash
    chmod +x conan_install.sh
    ./conan_install.sh
```

This will download and build all necessary libraries (Qt6, Crow, spdlog, etc.) for both Debug and Release configurations into the `build/` folder.

1. **Configure and Build the Project**:

```bash
# Configure using the Conan-generated toolchain
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build -j$(nproc)
```

4. **Run**:

```bash
./build/CakePlanner
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
