# Build Stage
FROM ubuntu:24.04 AS builder

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3-pip \
    python3-venv \
    pkg-config \
    qt6-base-dev \
    libqt6sql6-sqlite \
    libssl-dev \
    libgl1-mesa-dev \
    libx11-xcb-dev \
    libfontenc-dev \
    libxaw7-dev \
    libxcomposite-dev \
    libxcursor-dev \
    libxdamage-dev \
    libxfixes-dev \
    libxi-dev \
    libxinerama-dev \
    libxkbfile-dev \
    libxmu-dev \
    libxmuu-dev \
    libxpm-dev \
    libxrandr-dev \
    libxrender-dev \
    libxres-dev \
    libxss-dev \
    libxt-dev \
    libxtst-dev \
    libxv-dev \
    libxvmc-dev \
    libxxf86vm-dev \
    libxcb-render0-dev \
    libxcb-render-util0-dev \
    libxcb-xkb-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-shape0-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libxcb-dri3-dev \
    libxcb-cursor-dev \
    libxcb-util-dev \
    libxcb-util0-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Conan
RUN pip install conan --break-system-packages

WORKDIR /app
COPY . .

# Conan profile detection and installation
RUN conan profile detect --force && \
    conan install . --output-folder=build --build=missing -s build_type=Release

# Build with CMake
RUN cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

# Runtime Stage
FROM ubuntu:24.04

# Runtime-Abhängigkeiten for Qt (from system or Conan)
# Note: If Qt is installed via Conan, we might need to copy shared libraries.
# For simplicity, we assume some system libraries are still needed or static linking.
RUN apt-get update && apt-get install -y \
    libgl1 \
    libx11-xcb1 \
    libfontconfig1 \
    libdbus-1-3 \
    libxkbcommon0 \
    libxkbcommon-x11-0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy executable and potential Conan-deployed libraries
COPY --from=builder /app/build/CakePlanner .
# In a real scenario, you'd use a Conan deployer to copy all shared libs.

# Verzeichnis für Daten (Volume Mount Point)
RUN mkdir -p data

# Port freigeben
EXPOSE 8080

# Starten
CMD ["./CakePlanner"]