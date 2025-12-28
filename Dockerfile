# Build Stage
FROM ubuntu:24.04 AS builder

# Abhängigkeiten installieren (Compiler, CMake, Qt6, Git)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    libqt6sql6-sqlite \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Bauen
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Runtime Stage
FROM ubuntu:24.04

# Runtime-Abhängigkeiten für Qt6 und SQLite
RUN apt-get update && apt-get install -y \
    libqt6core6t64 \
    libqt6sql6 \
    libqt6sql6-sqlite \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Executable aus der Build-Stage kopieren
COPY --from=builder /app/build/CakePlanner .

# Verzeichnis für Daten (Volume Mount Point)
RUN mkdir -p data

# Port freigeben
EXPOSE 8080

# Starten
CMD ["./CakePlanner"]