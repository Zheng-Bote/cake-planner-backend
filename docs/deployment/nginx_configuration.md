# NGINX Configuration

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

**Table of Contents**

- [NGINX Configuration](#nginx-configuration)
  - [nginx.conf Example](#nginxconf-example)
  - [cake-planner.conf Example](#cake-plannerconf-example)
  - [check configuration](#check-configuration)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## nginx.conf Example

```nginx
user www;
worker_processes auto;
pid /webapp/run/nginx.pid;

events {
  worker_connections 768;
  multi_accept on;
}

http {

  ##
  # Basic Settings
  ##

  sendfile on;
  tcp_nopush on;
  tcp_nodelay on;
  keepalive_timeout 65;
  types_hash_max_size 2048;
  server_tokens off;

  server_names_hash_bucket_size 64;
  # server_name_in_redirect off;

  include /etc/nginx/mime.types;
  default_type application/octet-stream;

  access_log /webapp/var/logs/nginx/access.log;
  error_log /webapp/var/logs/nginx/error.log;

  gzip on;
  gzip_disable "msie6";

    # ZONE 1: API General
    # 10 Requests pro Sekunde pro IP. Speicher: 10MB (~160k IPs)
    limit_req_zone $binary_remote_addr zone=api_general_limit:10m rate=10r/s;
    # ZONE 2: Auth Strict
    # 1 Request pro Sekunde pro IP (oder sogar 30r/m). Speicher: 10MB
    limit_req_zone $binary_remote_addr zone=auth_strict_limit:10m rate=1r/s;

  include /webapp/etc/nginx/conf.d/*.conf;
  include /webapp/etc/nginx/sites-enabled/*;
}
```

## cake-planner.conf Example

```nginx
server {
    server_name www.cake-planner.digidocu.dev cake-planner.digidocu.dev;

    # --- CLEAN & STRICT CSP ---
    # Änderungen:
    # 1. font-src: Nur noch 'self' (kein Google mehr)
    # 2. style-src: Nur noch 'self' 'unsafe-inline' (kein Google mehr)
    # 3. img-src: 'self' + Google Avatare (bleibt nötig für Login-Bilder)
    set $csp_header "default-src 'self'; script-src 'self' 'unsafe-inline' https://apis.google.com; style-src 'self' 'unsafe-inline'; font-src 'self' data:; img-src 'self' https://*.googleusercontent.com data:; connect-src 'self' https://accounts.google.com https://*.googleapis.com;";

    add_header Content-Security-Policy $csp_header always;
    add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
    add_header X-Frame-Options "SAMEORIGIN";
    add_header X-Content-Type-Options "nosniff";
    add_header Referrer-Policy "strict-origin-when-cross-origin";
    add_header Permissions-Policy "geolocation=(), microphone=(), camera=()";
    add_header Cross-Origin-Opener-Policy "same-origin";
    add_header Cross-Origin-Embedder-Policy "require-corp";

    listen 443 ssl http2;
    ssl_certificate /etc/letsencrypt/live/cake-planner.digidocu.dev/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/cake-planner.digidocu.dev/privkey.pem;
    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;

    access_log /webapp/var/logs/nginx/cake-planner.access.log;
    error_log /webapp/var/logs/nginx/cake-planner.error.log;

    client_max_body_size 20M;
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml application/xml+rss text/javascript;

    location / {
        root /webapp/html/cake-planner/user-app;
        index index.html;
        try_files $uri $uri/ /index.html;
    }

    location /api/uploads/ {
        alias /webapp/html/cake-planner/public/uploads/;
        expires 30d;

        # Security Header wiederholen (Variable nutzen)
        add_header Cache-Control "public, no-transform";
        add_header Content-Security-Policy $csp_header always;
        add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
        add_header X-Frame-Options "SAMEORIGIN";
        add_header X-Content-Type-Options "nosniff";

        try_files $uri $uri/ =404;
    }

    location /api {
        proxy_pass http://172.17.0.1:8888;
        proxy_http_version 1.1;
        proxy_set_header Connection "";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_buffering off;
        proxy_cache off;
        proxy_read_timeout 24h;
    }
}

server {
    if ($host = www.cake-planner.digidocu.dev) { return 301 https://$host$request_uri; }
    if ($host = cake-planner.digidocu.dev) { return 301 https://$host$request_uri; }
    listen 80;
    server_name www.cake-planner.digidocu.dev cake-planner.digidocu.dev;
    return 404;
}
```

## check configuration

```bash
nginx -t && service nginx restart
```

```bash
curl https://cake-planner.digidocu.dev/login -s -I -H "secret-header:true"
```

**Expected:**

```http
HTTP/2 200
server: nginx
date: Sun, 25 Jan 2026 09:52:56 GMT
content-type: text/html
content-length: 55453
last-modified: Sun, 25 Jan 2026 09:43:39 GMT
etag: "6975e5cb-d89d"
content-security-policy: default-src 'self'; script-src 'self' 'unsafe-inline' https://apis.google.com; style-src 'self' 'unsafe-inline'; font-src 'self' data:; img-src 'self' https://*.googleusercontent.com data:; connect-src 'self' https://accounts.google.com https://*.googleapis.com;
strict-transport-security: max-age=31536000; includeSubDomains
x-frame-options: SAMEORIGIN
x-content-type-options: nosniff
referrer-policy: strict-origin-when-cross-origin
permissions-policy: geolocation=(), microphone=(), camera=()
cross-origin-opener-policy: same-origin
cross-origin-embedder-policy: require-corp
accept-ranges: bytes
```
