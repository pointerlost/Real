# vcpkg
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
            "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file"
    )
endif()

cmake_minimum_required(VERSION 3.21)

project(real LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(engine)
add_subdirectory(apps/editor)
add_subdirectory(apps/game)