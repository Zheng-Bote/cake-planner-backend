<!-- DOCTOC SKIP -->
# nginx.conf Example

```nginx
events {
    worker_connections 1024;
}

http {
    include       mime.types;
    default_type  application/octet-stream;

    server {
        listen 80;
        server_name localhost;

        # Forward all requests to Crow Backend
        location / {
            proxy_pass http://cakeplanner:8080;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
        }

        # Optional: Static File Caching for uploads
        location /public/ {
             alias /usr/share/nginx/html/public/;
             expires 30d;
        }
    }
}
```

