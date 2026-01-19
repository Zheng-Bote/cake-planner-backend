<!-- DOCTOC SKIP -->
# docker-compose.yml example

```yaml
services:
  cakeplanner:
    build:
      context: .
      dockerfile: Dockerfile.appimage
    container_name: cakeplanner_app
    volumes:
      - ./data:/app/data
      - ./public:/app/public
      - ./logs:/app/logs
    env_file:
      - CakePlanner.env
    restart: unless-stopped
    expose:
      - "8080"

  nginx:
    build:
      context: .
      dockerfile: Dockerfile.nginx
    container_name: cakeplanner_nginx
    ports:
      - "80:80"
    depends_on:
      - cakeplanner
    restart: unless-stopped
```
