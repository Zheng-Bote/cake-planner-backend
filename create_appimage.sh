#!/usr/bin/bash
set -e  # Script sofort beenden, wenn ein Fehler passiert

# --- KONFIGURATION ---
APP_NAME="CakePlanner"
BUILD_DIR="build"
APP_DIR="AppDir"
EXECUTABLE_NAME="CakePlanner" # Wie heißt dein Binary in CMakeLists.txt?
ICON_PATH="logo_256x256.png" # Kuchen-Icon

# 1. Build Verzeichnis sauber machen und neu bauen mit Conan
echo "🍰 Baue das Projekt mit Conan..."
if [ -d "$BUILD_DIR" ]; then rm -rf "$BUILD_DIR"; fi

# Conan Installation
conan profile detect --force || true
conan install . --output-folder="$BUILD_DIR" --build=missing -s build_type=Release

# CMake Konfiguration und Build
cmake -S . -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$BUILD_DIR/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "$BUILD_DIR" -j$(nproc)

# 2. AppDir Vorbereiten (Die Ordnerstruktur des AppImages)
echo "📁 Erstelle AppDir Struktur..."
if [ -d "$APP_DIR" ]; then rm -rf "$APP_DIR"; fi
mkdir -p "$APP_DIR"/usr/bin
mkdir -p "$APP_DIR"/usr/share/icons/hicolor/256x256/apps/
mkdir -p "$APP_DIR"/usr/share/applications/

# Binary kopieren
cp "$BUILD_DIR/$EXECUTABLE_NAME" "$APP_DIR/usr/bin/$APP_NAME"

# 3. Assets kopieren (WICHTIG!)
# Wir kopieren den 'public' Ordner NICHT in das AppImage, da du dort reinschreiben willst (Uploads).
# AppImages sind read-only! Aber wir kopieren statische Assets falls nötig.
# Fürs erste gehen wir davon aus, dass der User 'public' neben das AppImage legt.

# 4. Metadaten erstellen (.desktop Datei)
echo "📝 Erstelle .desktop Datei..."
cat > "$APP_DIR/usr/share/applications/$APP_NAME.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=CakePlanner
Comment=Organize your baking events
Exec=$APP_NAME
Icon=$APP_NAME
Categories=Office;
Terminal=true
EOF

# 5. Icon besorgen
echo "🖼️ Lade Icon...$ICON_PATH"
# wget -q "$ICON_URL" -O "$APP_DIR/usr/share/icons/hicolor/256x256/apps/$APP_NAME.png"
cp "$ICON_PATH" "$APP_DIR/usr/share/icons/hicolor/256x256/apps/$APP_NAME.png"
# Auch ins Root legen, damit AppImage es findet
cp "$APP_DIR/usr/share/icons/hicolor/256x256/apps/$APP_NAME.png" "$APP_DIR/$APP_NAME.png"

# 6. Linuxdeploy Tools holen
echo "🔧 Lade Linuxdeploy und Qt-Plugin..."
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget -q "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    wget -q "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
fi

# 7. AppImage generieren
echo "📦 Generiere AppImage (inklusive Qt Dependencies)..."

# Umgebungsvariable für Qt Plugin setzen (sucht automatisch nach qmake)
export QMAKE=$(which qmake6 || which qmake)
export EXTRA_QT_PLUGINS="iconengines,imageformats,sqldrivers,platforms"

./linuxdeploy-x86_64.AppImage \
    --appdir "$APP_DIR" \
    --executable "$APP_DIR/usr/bin/$APP_NAME" \
    --desktop-file "$APP_DIR/usr/share/applications/$APP_NAME.desktop" \
    --icon-file "$APP_DIR/$APP_NAME.png" \
    --plugin qt \
    --output appimage

echo "✅ Fertig! Dein AppImage liegt hier: CakePlanner-x86_64.AppImage"
