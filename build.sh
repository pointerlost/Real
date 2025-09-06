#!/usr/bin/env bash
set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
cd "$SCRIPT_DIR"

echo "[INFO] Updating git submodules..."
git submodule update --init --recursive

# vcpkg
if [ ! -d "opengl_renderer/vcpkg" ]; then
    echo "[INFO] Cloning vcpkg..."
    git clone https://github.com/microsoft/vcpkg.git opengl_renderer/vcpkg
    ./opengl_renderer/vcpkg/bootstrap-vcpkg.sh
fi

echo "[INFO] Installing dependencies via vcpkg..."
./opengl_renderer/vcpkg/vcpkg install --triplet x64-linux

# Build
mkdir -p opengl_renderer/build
cd opengl_renderer/build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .

