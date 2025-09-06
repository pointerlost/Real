@echo off
setlocal enabledelayedexpansion
cd /d %~dp0

echo [INFO] Updating git submodules...
git submodule update --init --recursive

if not exist opengl_renderer\vcpkg (
    echo [INFO] Cloning vcpkg...
    git clone https://github.com/microsoft/vcpkg.git opengl_renderer\vcpkg
    call opengl_renderer\vcpkg\bootstrap-vcpkg.bat
)

echo [INFO] Installing dependencies via vcpkg...
opengl_renderer\vcpkg\vcpkg.exe install --triplet x64-windows

if not exist opengl_renderer\build mkdir opengl_renderer\build
cd opengl_renderer\build

cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -A x64
cmake --build . --config Debug

