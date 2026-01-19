<!-- DOCTOC SKIP -->
# Dockerfile.nginx Example

```dockerfile
FROM nginx:alpine

COPY nginx.conf /etc/nginx/nginx.conf

# Optional: Copy static files if you mount them differently
# COPY public /usr/share/nginx/html/public
```

