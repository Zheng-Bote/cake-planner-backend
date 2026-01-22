<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Dockerfile.appimage Example](#dockerfileappimage-example)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Dockerfile.appimage Example

```dockerfile
# Base Image: Ubuntu 24.04 (Matches Build System)
FROM ubuntu:24.04

# Install FUSE and dependencies for AppImage
# --appimage-extract-and-run avoids needing actual FUSE kernel module,
# but libfuse2 is often still checked by runtime.
RUN apt-get update && apt-get install -y \
    libfuse2 \
    file \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the pre-built AppImage
# IMPORTANT: You must build the AppImage first using create_appimage.sh!
COPY CakePlanner-x86_64.AppImage .

# Ensure it's executable
RUN chmod +x CakePlanner-x86_64.AppImage

# Create working directories
RUN mkdir -p data public logs

EXPOSE 8080

# Run AppImage with special flag to work in Docker without FUSE privileges
CMD ["./CakePlanner-x86_64.AppImage", "--appimage-extract-and-run"]
```

