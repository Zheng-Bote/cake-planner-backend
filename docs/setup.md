# CakePlanner Setup

## Voraussetzungen

Ich gehe von folgender Ordnerstruktur auf dem Server aus (du kannst die Pfade natürlich anpassen):

- User App: `/var/www/cake-planner/user-app`
- Admin Panel: `/var/www/cake-planner/admin-panel`
- Backend: Läuft auf `http://127.0.0.1:8080`

**Backend Upload-Ordner**

Vergiss nicht, dass NGINX und das C++ Backend auf denselben Ordner zugreifen müssen, falls du statische Dateien direkt ausliefern willst. In meiner Konfiguration oben übernimmt das Backend (proxy_pass) die Auslieferung der Bilder über die Route `/api/uploads`. Das ist sicherer und einfacher, da die C++ Logik Pfad-Validierung betreibt.

## NGINX

### NGINX Configuration

Datei, z.B. `/etc/nginx/sites-available/cake-planner`

```nginx
server {
    listen 80;
    listen [::]:80;
    server_name www.cake-planner.digidocu.dev;

    # Redirect HTTP to HTTPS (empfohlen, sobald Certbot läuft)
    # return 301 https://$host$request_uri;

    # Logs
    access_log /var/log/nginx/cake-planner.access.log;
    error_log /var/log/nginx/cake-planner.error.log;

    # Globale Einstellungen für Uploads (Wichtig für Bilder!)
    # Dein Backend erlaubt Uploads, Nginx standardmäßig nur 1MB.
    client_max_body_size 20M;

    # GZIP Komprimierung (Wichtig für Angular Performance)
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml application/xml+rss text/javascript;

    # --------------------------------------------------------
    # 1. USER APP (Root /)
    # --------------------------------------------------------
    location / {
        root /var/www/cake-planner/user-app;
        index index.html;

        # WICHTIG für Angular Routing:
        # Wenn eine Datei nicht existiert (z.B. /dashboard),
        # leite an index.html weiter, damit Angular das Routing übernimmt.
        try_files $uri $uri/ /index.html;
    }

    # --------------------------------------------------------
    # 2. ADMIN PANEL (Subpath /admin)
    # --------------------------------------------------------
    location /admin {
        # 'alias' ist wichtig für Sub-Pfade
        alias /var/www/cake-planner/admin-panel;
        index index.html;

        # Routing Fallback für das Admin Panel
        try_files $uri $uri/ /admin/index.html;
    }

    # --------------------------------------------------------
    # 3. BACKEND API & UPLOADS (/api)
    # --------------------------------------------------------
    location /api {
        proxy_pass http://127.0.0.1:8080; # Port deines C++ Backends

        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        # ----------------------------------------------------
        # WICHTIG für Server-Sent Events (SSE) / Stream
        # Siehe event_controller.cpp: /api/events/stream
        # ----------------------------------------------------
        # Deaktiviert Buffering, damit Events sofort beim Client ankommen
        proxy_buffering off;
        proxy_cache off;

        # Erhöht Timeout für Long-Polling Verbindungen (Standard ist oft 60s)
        proxy_read_timeout 24h;
    }
}
```

### NGINX Enable

```bash
sudo ln -s /etc/nginx/sites-available/cake-planner /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

### SSL/HTTPS (Certbot)

```bash
sudo apt install certbot python3-certbot-nginx
sudo certbot --nginx -d cake-planner.digidocu.dev
```

## Angular Frontends

**Angular Build Parameter**

Damit das Routing unter / und /admin funktioniert, musst du die Apps mit der korrekten base-href bauen:

### User App

```bash
ng build --project user-app --base-href /  --configuration production
```

Die Dateien werden in der `dist`-Ordner gespeichert.

### Admin Panel

_Ganz wichtig, damit /admin funktioniert!_

```bash
ng build --project admin-panel --base-href /admin  --configuration production
```

## Crow Backend

Da ein AppImage normalerweise darauf ausgelegt ist, direkt auf einem Linux-Host zu laufen (Desktop), ist der Betrieb in einem Docker-Container etwas speziell.

Das Hauptproblem: AppImages nutzen FUSE, um sich selbst zu mounten. Docker-Container haben standardmäßig keinen Zugriff auf FUSE (dafür bräuchte man --privileged, was unsicher ist).

Die Lösung: Wir extrahieren das AppImage im Dockerfile (--appimage-extract). Dadurch erhalten wir einen normalen Ordner mit dem Programm, das wir ohne spezielle Rechte ausführen können.

### Dockerfile

Hier ist das Dockerfile und die passende Environment-Datei.

```dockerfile
# Wir nutzen ein aktuelles, schlankes Ubuntu als Basis, da das AppImage
# vermutlich auf einer ähnlichen glibc-Version gebaut wurde.
FROM ubuntu:22.04

# Umgebungsvariablen für die App setzen
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

# 1. Notwendige Laufzeit-Bibliotheken installieren
# Auch wenn das AppImage vieles mitbringt, fehlen in "slim" Docker-Images oft
# Basics wie libglib oder ssl Zertifikate.
RUN apt-get update && apt-get install -y \
    ca-certificates \
    libglib2.0-0 \
    libsm6 \
    libxext6 \
    libxrender-dev \
    && rm -rf /var/lib/apt/lists/*

# Arbeitsverzeichnis erstellen
WORKDIR /app

# 2. AppImage kopieren
# Hinweis: Die Datei muss im selben Ordner liegen wie dieses Dockerfile!
COPY CakePlanner-x86_64.AppImage /app/server.AppImage

# 3. AppImage entpacken (WICHTIG!)
# Das umgeht die Notwendigkeit für FUSE im Docker-Container.
# Es erstellt den Ordner "squashfs-root".
RUN chmod +x /app/server.AppImage && \
    /app/server.AppImage --appimage-extract && \
    rm /app/server.AppImage

# 4. Ordner-Struktur für Persistenz erstellen
# Laut deinem Code (event_controller.cpp) nutzt du "public/uploads".
RUN mkdir -p /app/public/uploads && \
    mkdir -p /app/data

# 5. User erstellen (FIX: Konflikt mit UID 1000 lösen)
# Das Image ubuntu:24.04 hat bereits einen User 'ubuntu' mit UID 1000.
# Wir löschen ihn, um die ID für unseren 'cakeuser' freizumachen.
RUN userdel -r ubuntu || true && \
    useradd -m -u 1000 cakeuser && \
    chown -R cakeuser:cakeuser /app

# Zum User wechseln
USER cakeuser

# 6. Ports exponieren
# Crow läuft standardmäßig oft auf 18080 oder 8080 (im Code prüfen!)
EXPOSE 8080

# 7. Startbefehl
# Wir starten den "AppRun" im entpackten Ordner.
# WICHTIG: Wir setzen UPLOAD_DIR via ENV, damit es auf das Volume zeigt.
CMD ["./squashfs-root/AppRun"]
```

### CakePlanner.env

Diese Datei steuert die Konfiguration.

```env
# CakePlanner.env

# Server
CAKE_SERVER_PORT=8080
CAKE_ADMIN_PASSWORD=TopSecret123!
CAKE_JWT_SECRET=MeinSuperGeheimesLangesSecretFürJWTs

# SMTP
SMTP_SERVER="smtp.server.com"
SMTP_PORT="587"
SMTP_USERNAME="admin@server.com"
SMTP_PASSWORD="TopSecret123!"
SMTP_FROM="admin@server.com"
SMTP_STARTTLS="true"

# Filesystem (INTERNAL DOCKER PATHS)
## Logging
LOG_DIR=/app/logs
LOG_LEVEL=info

## Photo Uploads
UPLOAD_DIR=/app/public/uploads

## Database
DB_DIR=/app/data/cakeplanner.sqlite
```

## Build & Run Befehle

Um das Ganze zu starten, hier die Befehle für das Terminal.

### Image bauen

Stelle sicher, dass CakePlanner-x86_64.AppImage, Dockerfile und CakePlanner.env im selben Ordner liegen.

```bash
docker build -t cake-planner-backend .
```

### Container starten

Hier verbinden wir die Volumes, damit Uploads und die Datenbank einen Neustart des Containers überleben.

```bash
docker run -d \
  --name cake-planner-backend \
  -p 8080:8080 \
  -v /var/www/cake-planner/data:/app/data \
  -v /var/www/cake-planner/public:/app/public \
  -v /var/www/cake-planner/CakePlanner.env:/app/CakePlanner.env \
  cake-planner-backend
```

**checks**

initial admin: `admin@cakeplanner.local`
Password: see CakePlanner.env (`CAKE_ADMIN_PASSWORD`)

```bash
docker logs -f cake-planner-backend
```

```bash
netstat -tulpn | grep 8080
```

```bash
curl http://localhost:8080/api/profile
```

_this should produce an error (`Invalid credentials`):_

```bash
curl -v -X POST http://127.0.0.1:8888/api/login \
     -H "Content-Type: application/json" \
     -d '{"email":"admin@example.com", "password":"admin"}'
```

```bash
docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' cake-planner-backend
```

old Debian Kernel (old Ubuntu, eg Kernel 4.19)
ufw allow 8888/tcp
ufw allow in on docker0

_later:_

```bash
docker start cake-planner-backend
```

**Erklärung der Volumes:**

- -v $(pwd)/uploads:/app/public/uploads: Die Bilder landen im lokalen uploads Ordner.
- -v $(pwd)/cake_data:/app/data: Die cake.db landet im lokalen cake_data Ordner.

Damit läuft das Backend isoliert, sicher und mit persistenter Datenhaltung! 🐳🍰

### Container stoppen

```bash
docker stop cake-planner-backend
docker rm cake-planner-backend
```
