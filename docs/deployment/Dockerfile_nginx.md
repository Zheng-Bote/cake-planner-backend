<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Dockerfile.nginx Example](#dockerfilenginx-example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Dockerfile.nginx Example

```dockerfile
FROM nginx:alpine

COPY nginx.conf /etc/nginx/nginx.conf

# Optional: Copy static files if you mount them differently
# COPY public /usr/share/nginx/html/public
```

