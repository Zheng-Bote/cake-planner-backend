#!/bin/bash
# conan_install.sh

# Detect or create default profile
if ! conan profile show default > /dev/null 2>&1; then
    echo "Detecting default Conan profile..."
    conan profile detect --force
fi

# Install dependencies using Conan 2
conan install . --output-folder=build --build=missing -s build_type=Release -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
conan install . --output-folder=build --build=missing -s build_type=Debug -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True

echo "Conan installation complete. You can now build the project using:"
echo "cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release"
echo "cmake --build build"
