<!-- DOCTOC SKIP -->

# Deployment Overview

## Components

### Webserver

NGINX running in Docker or Podman. Proxy for Web-Applicationserver.

### Web-Applicationserver

Qt6 C++23 Crow-Server; running in Docker or Podman.

### Database

SQLite3 db within Web-Applicationserver.

### Frontends

#### Admin Panel

SPA with Angular v21 and Angular Material v21.

Provided by the Webserver.

Functions for admin users:

- user account
  - activations
  - deactivations
  - deletions
  - push password reset for users
  - assign users to groups
- groups
  - creation
  - deletion

#### User Frontend

SPA with Angular v21 and Material v21

Provided by the Webserver.

Functions for users:

- Dashboard
- Calendar
- Hall of Fame
- Profile
- System Info
