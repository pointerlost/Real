@echo off
setlocal enabledelayedexpansion
cd /d %~dp0

echo [INFO] Updating git submodules...
git submodule update --init --recursive

if not exist engine\vcpkg (
    echo [INFO] Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git engine\vcpkg
    call engine\vcpkg\bootstrap-vcpkg.bat
)

echo [INFO] Installing dependencies via vcpkg...
engine\vcpkg\vcpkg.exe install --triplet x64-windows

if not exist engine\build mkdir engine\build
cd engine\build

cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
cmake --build . --config Debug

