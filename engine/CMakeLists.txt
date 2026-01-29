cmake_minimum_required(VERSION 3.21)

# manifest mode
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file"
    )
endif()

project(real)
set(ENGINE_TARGET engine)
set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

find_package(OpenGL REQUIRED)
find_package(zstd CONFIG REQUIRED)

add_executable(ENGINE_TARGET
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
    include/Core/RealConfig.h
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
    include/Scene/Systems/Systems.h
    src/Scene/Systems/Systems.cpp
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
    src/Graphics/ModelLoader.cpp
    include/Graphics/ModelLoader.h
    include/Common/RealEnum.h
    include/Graphics/Model.h
    include/Resource/ResourceLoader.h
    include/Common/RealTypes.h
    src/Resource/ResourceLoader.cpp
    include/Serialization/Json.h
    src/Serialization/Json.cpp
    include/Serialization/Binary.h
    src/Serialization/Binary.cpp
    include/Core/AssetImporter.h
    src/Core/AssetImporter.cpp
    src/Graphics/Model.cpp
    include/Core/CMakeConfig.h
    include/Math/Vec3.h
    include/Math/Vec2.h
    include/Math/Vec4.h
    include/Math/Mat2.h
    include/Math/Mat3.h
    include/Math/Mat4.h
    include/Math/Quat.h
    src/Math/Quat.cpp
    src/Math/Mat2.cpp
    src/Math/Mat3.cpp
    src/Math/Mat4.cpp
    src/Math/Vec2.cpp
    src/Math/Vec3.cpp
    src/Math/Vec4.cpp
    include/Scene/Systems/PhysicsSystem.h
    include/Physics/PhysXContext.h
    src/Physics/PhysXContext.cpp
    include/Physics/PhysXTypes.h
    src/Scene/Systems/PhysicsSystem.cpp
    include/Scene/Systems/MeshRendererSystem.h
    include/Scene/Systems/CameraSystem.h
    include/Scene/Systems/LightSystem.h
    src/Scene/Systems/MeshRendererSystem.cpp
    src/Scene/Systems/CameraSystem.cpp
    src/Scene/Systems/LightSystem.cpp
    include/Physics/PhysXErrorCallback.h
    src/Physics/PhysXErrorCallback.cpp
    src/Physics/PhysXAllocator.cpp
    include/Physics/PhysXAllocator.h
    include/Physics/PhysicsTypes.h
        include/Scene/Systems/MovementSystem.h
        src/Scene/Systems/MovementSystem.cpp
        include/Physics/PhysicsAPI.h
        include/Event/Signal.h
        include/Event/SceneEvents.h
        src/Event/Signal.cpp
        src/Event/SceneEvents.cpp
)

set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)
set(ASSETS_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/assets/)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/Core/CMakeConfig.h
    @ONLY
)

target_sources(ENGINE_TARGET
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include/Core/CMakeConfig.h
)

add_library(glad_local STATIC vendor/glad/src/glad.c)

target_include_directories(glad_local PUBLIC vendor/glad/include)

target_include_directories(ENGINE_TARGET
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx/include
)

set(PHYSX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/physx)

target_link_libraries(ENGINE_TARGET
    PRIVATE
    glad_local
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
    assimp::assimp
    zstd::libzstd

    # AMD Compressonator lib
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

    # PhysX
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysX_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXExtensions_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXCommon_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXFoundation_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysX_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXExtensions_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXCommon_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXFoundation_static_64.a>
)
if(UNIX AND NOT APPLE)
    target_link_libraries(ENGINE_TARGET PRIVATE pthread dl)
endif()