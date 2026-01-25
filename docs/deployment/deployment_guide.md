# Deployment Guide

This guide describes how to deploy the Cake Planner Backend using **Docker Compose**, **Nginx**, and the **AppImage**.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Deployment Guide](#deployment-guide)
  - [Architecture](#architecture)
  - [Prerequisites](#prerequisites)
  - [Configuration Files](#configuration-files)
  - [Step-by-Step Deployment](#step-by-step-deployment)
    - [1. Build the AppImage](#1-build-the-appimage)
    - [2. Configure Environment](#2-configure-environment)
    - [3. Start Services](#3-start-services)
    - [4. Verify](#4-verify)
  - [Notes](#notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Architecture

The deployment consists of two containers:

1. **cakeplanner**: Runs the backend application via AppImage.
2. **nginx**: Acts as a Reverse Proxy, forwarding specific ports (e.g., 80 or 443) to the backend.

## Prerequisites

- Docker & Docker Compose installed.
- A built `CakePlanner-x86_64.AppImage` (placed in this directory).

## Configuration Files

The `docs/deployment/` directory contains all necessary files:

- `docker-compose.yml`: Orchestration file.
- `nginx.conf`: Nginx configuration.
- `Dockerfile.appimage`: Dockerfile to containerize the AppImage.
- `Dockerfile.nginx`: Dockerfile for the proxy.
- `CakePlanner.env.example`: Template for environment variables.

## Step-by-Step Deployment

### 1. Build the AppImage

First, ensure you have built the project `create_appimage.sh` (see [Build Process](../development/build_process.md)).
Copy the resulting `CakePlanner-x86_64.AppImage` into this deployment folder:

```bash
cp ../../CakePlanner-x86_64.AppImage .
```

### 2. Configure Environment

Create a `.env` file from the example:

```bash
cp CakePlanner.env.example CakePlanner.env
nano CakePlanner.env
```

*Adjust SMTP settings and other variables as needed.*

### 3. Start Services

Run Docker Compose to build and start the containers:

```bash
docker-compose up -d --build
```

### 4. Verify

- **Backend**: Accessible internally at `http://cakeplanner:8080`.
- **Frontend/Public**: Accessible via Nginx at `http://localhost:80`.

## Notes

- **Data Persistence**: The `data/` folder is mounted as a volume, so the SQLite database persists across restarts.
- **Uploads**: `public/` is also mounted to persist uploaded images.
- **AppImage in Docker**: We use the `--appimage-extract-and-run` flag to run the AppImage inside the container without requiring privileged FUSE access.
