cmake_minimum_required(VERSION 3.21)
# manifest mode
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
project(opengl_renderer)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(zstd CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    main/main.cpp
    src/Core/file_manager.cpp
    include/Core/Window.h
    src/Core/Window.cpp
    include/Core/Logger.h
    src/Core/Engine.cpp
    include/Core/Engine.h
    include/Core/Utils.h
    include/Core/Timer.h
    src/Core/Timer.cpp
    include/Graphics/Shader.h
    src/Graphics/Shader.cpp
    include/Core/AssetManager.h
    src/Core/AssetManager.cpp
    include/Graphics/Mesh.h
    include/Graphics/MeshManager.h
    src/Graphics/MeshManager.cpp
    include/Core/Services.h
    src/Core/Services.cpp
    include/Graphics/MeshFactory.h
    src/Graphics/MeshFactory.cpp
    include/Graphics/Texture.h
    include/Graphics/Transformations.h
    include/Graphics/Camera.h
    src/Graphics/Camera.cpp
    include/Core/Config.h
    include/Graphics/Renderer.h
    src/Graphics/Renderer.cpp
    include/Scene/Scene.h
    src/Scene/Scene.cpp
    include/Input/Input.h
    src/Input/Input.cpp
    include/Core/Callback.h
    src/Core/Callback.cpp
    include/Input/Keycodes.h
    include/Input/CameraInput.h
    src/Input/CameraInput.cpp
    include/Scene/Components.h
    include/Scene/Entity.h
    include/Core/UUID.h
    src/Core/UUID.cpp
    src/Scene/Entity.cpp
    include/Editor/EditorState.h
    src/Editor/EditorState.cpp
    src/Graphics/Transformations.cpp
    include/Graphics/Light.h
    src/Graphics/Light.cpp
    include/Graphics/Material.h
    src/Graphics/Texture.cpp
    include/Graphics/Buffer.h
    src/Graphics/Buffer.cpp
    include/Graphics/GPUBuffers.h
    include/Graphics/RenderContext.h
    src/Graphics/RenderContext.cpp
    include/Graphics/RenderCommand.h
    src/Graphics/Material.cpp
    src/Editor/EditorPanel.cpp
    include/Editor/EditorPanel.h
    include/Editor/HierarchyPanel.h
    include/Editor/InspectorPanel.h
    include/Editor/IPanel.h
    src/Editor/HierarchyPanel.cpp
    src/Editor/InspectorPanel.cpp
    vendor/ImGuizmo/ImGuizmo.cpp
    include/Math/Math.h
    src/Math/Math.cpp
    include/Scene/Systems.h
    include/Scene/SystemUpdate.h
    src/Scene/SystemUpdate.cpp
    src/Scene/Systems.cpp
    include/API/OpenGL/Types/FrameBuffer.h
    src/API/OpenGL/Types/FrameBuffer.cpp
    include/Util/Util.h
    src/Util/Util.cpp
    include/Tools/ImageTools.h
    src/Tools/ImageTools.cpp
    include/Graphics/TextureArrays.h
    src/Graphics/TextureArrays.cpp
    include/Common/Scheduling/Threads.h
    src/Common/Scheduling/Threads.cpp
    include/Common/Macros.h
    include/Core/Types.h
    include/Common/Scheduling/TaskManager.h
)

add_library(glad_local STATIC vendor/glad/src/glad.c)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

target_include_directories(glad_local PUBLIC vendor/glad/include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glad_local
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
    assimp::assimp
    zstd::libzstd

    # Compressonator
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Compressonator.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Framework.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Core.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Common.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libImage_DDS.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libImage_KTX.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libImage_EXR.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libImage_TGA.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Core_SSE.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Core_AVX.a
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/compressonator/lib/libCMP_Core_AVX512.a
)

set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/)

configure_file(
    include/Core/Config.h.in
    include/Core/CmakeConfig.h
)