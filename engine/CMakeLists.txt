project(engine)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(zstd CONFIG REQUIRED)

add_library(engine STATIC
    include/Core/IApplication.h
    include/Graphics/RenderTypes.h
    include/Math/iVec2.h
    src/Math/iVec2.cpp
    include/Graphics/FrameBuilder.h
    src/Graphics/FrameBuilder.cpp
    include/Graphics/Texture/TextureTypes.h
    include/Scene/SceneManager.h
    src/Scene/SceneManager.cpp
    include/Core/IApplicationContext.h
        include/RHI/RHITypes.h
        include/Assets/TextureManager.h
        include/Assets/ShaderManager.h
        include/Assets/MaterialFactory.h
        src/Assets/TextureManager.cpp
        src/Assets/MaterialFactory.cpp
        src/Assets/ShaderManager.cpp
        include/Assets/AssetSystem.h
        include/Assets/AssetTypes.h
        include/Platform/OpenGL/OpenGLTypes.h
        include/RHI/ITexture.h
)

target_sources(engine PRIVATE
    include/RHI/IRenderDevice.h
    src/Platform/Vulkan/VkRenderer.cpp
    include/Platform/Vulkan/VkRenderer.h
    src/Platform/Vulkan/VkRenderDevice.cpp
    include/Platform/Vulkan/VkRenderDevice.h
    src/Assets/FileManager.cpp
    include/Core/Logger.h
    include/Core/Utils.h
    include/Timer/Timer.h
    src/Timer/Timer.cpp
    include/Graphics/Shader.h
    src/Graphics/Shader.cpp
    include/Assets/AssetManager.h
    src/Assets/AssetManager.cpp
    include/Graphics/MeshManager.h
    src/Graphics/MeshManager.cpp
    include/Core/Services.h
    src/Core/Services.cpp
    include/Graphics/MeshFactory.h
    src/Graphics/MeshFactory.cpp
    include/Graphics/Texture/Texture.h
    include/Graphics/Transformations.h
    include/Graphics/Camera.h
    src/Graphics/Camera.cpp
    include/Core/RealConfig.h
    include/Scene/Scene.h
    src/Scene/Scene.cpp
    include/Input/Input.h
    src/Input/Input.cpp
    include/Input/Keycodes.h
    include/Input/CameraInput.h
    src/Input/CameraInput.cpp
    include/Scene/Components.h
    include/Scene/Entity.h
    include/Core/UUID.h
    src/Core/UUID.cpp
    src/Scene/Entity.cpp
    src/Graphics/Transformations.cpp
    include/Graphics/Light.h
    src/Graphics/Light.cpp
    include/Graphics/Material.h
    src/Graphics/Texture/Texture.cpp
    include/Graphics/Buffer.h
    src/Graphics/Buffer.cpp
    include/Graphics/GPUBuffers.h
    include/Graphics/RenderContext.h
    src/Graphics/RenderContext.cpp
    include/Graphics/RenderCommand.h
    src/Graphics/Material.cpp
    ${CMAKE_SOURCE_DIR}/vendor/ImGuizmo/ImGuizmo.cpp
    include/Math/Math.h
    src/Math/Math.cpp
    include/Scene/Systems/SystemsManager.h
    src/Scene/Systems/SystemsManager.cpp
    include/Platform/OpenGL/FrameBuffer.h
    src/Platform/OpenGL/FrameBuffer.cpp
    include/Util/Util.h
    src/Util/Util.cpp
    include/Tools/ImageTools.h
    src/Tools/ImageTools.cpp
    include/Graphics/Texture/TextureArrays.h
    src/Graphics/Texture/TextureArrays.cpp
    include/Common/Scheduling/Threads.h
    src/Common/Scheduling/Threads.cpp
    include/Common/Macros.h
    include/Common/Scheduling/TaskManager.h
    src/Graphics/ModelLoader.cpp
    include/Graphics/ModelLoader.h
    include/Common/RealEnum.h
    include/Graphics/Model.h
    include/Assets/ResourceLoader.h
    include/Common/Types.h
    src/Assets/ResourceLoader.cpp
    include/Serialization/Json.h
    src/Serialization/Json.cpp
    include/Serialization/Binary.h
    src/Serialization/Binary.cpp
    include/Assets/AssetImporter.h
    src/Assets/AssetImporter.cpp
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
    include/Physics/Physx/PhysXTypes.h
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
    include/Event/Event.h
    include/Event/SceneEvents.h
    src/Event/Event.cpp
    src/Event/SceneEvents.cpp
    include/Graphics/Debug/DebugRenderer.h
    src/Graphics/Debug/DebugRenderer.cpp
    include/Graphics/Debug/DebugMeshFactory.h
    src/Graphics/Debug/DebugMeshFactory.cpp
    include/Graphics/Debug/DebugTypes.h
    include/Math/Conversions/GLMConversions.h
    src/Math/Conversions/GLMConversions.cpp
    include/Core/IPhysicsBackend.h
    include/Physics/Physx/PhysXBackend.h
    src/Physics/PhysX/PhysXBackend.cpp
    include/Physics/PhysicsDescriptors.h
    include/Scene/ISceneListener.h
    include/Core/ISystem.h
    include/Window/IWindow.h
    include/Core/Engine/EngineCore.h
    src/Core/Engine/EngineCore.cpp
    include/Platform/OpenGL/OpenGLRenderer.h
    src/Platform/OpenGL/OpenGLRenderer.cpp
    include/RHI/IRenderer.h
    include/Core/Engine/EngineBootstrap.h
    include/Platform/GLFW/GLFWwindow.h
    src/Platform/GLFW/GLFWwindow.cpp
    include/Core/ITimer.h
    src/Core/Engine/EngineBootstrap.cpp
    include/Window/WindowConfig.h
    include/Core/IPlatform.h
    include/Platform/GLFW/GLFWPlatform.h
    src/Platform/GLFW/GLFWPlatform.cpp
    include/Platform/OpenGL/OpenGLRenderDevice.h
    src/Platform/OpenGL/OpenGLRenderDevice.cpp
)

set(SHADERS_DIR shaders)
set(ASSETS_DIR assets)

configure_file(
    include/Core/Config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/Core/CMakeConfig.h
    @ONLY
)

target_sources(engine PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include/Core/CMakeConfig.h
)

add_library(glad_local STATIC ${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c)
target_include_directories(glad_local PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

target_include_directories(engine PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/physx/include
)

set(PHYSX_DIR ${CMAKE_SOURCE_DIR}/vendor/physx)
set(CMP_DIR   ${CMAKE_SOURCE_DIR}/vendor/compressonator/lib)

target_link_directories(engine PUBLIC
    ${CMP_DIR}
)

target_link_libraries(engine
    PUBLIC
    glad_local
    glfw
    glm::glm
    imgui::imgui
    nlohmann_json::nlohmann_json
    assimp::assimp
    OpenGL::GL
    zstd::libzstd
    -lCMP_Compressonator
    -lCMP_Framework
    -lCMP_Core
    -lCMP_Common
    -lImage_DDS
    -lImage_KTX
    -lImage_EXR
    -lImage_TGA
    -lCMP_Core_SSE
    -lCMP_Core_AVX
    -lCMP_Core_AVX512

    PRIVATE
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysX_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXPvdSDK_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/release/libPhysXExtensions_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXCommon_static_64.a>
    $<$<CONFIG:Debug>:${PHYSX_DIR}/lib/debug/libPhysXFoundation_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysX_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXExtensions_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXCommon_static_64.a>
    $<$<CONFIG:Release>:${PHYSX_DIR}/lib/release/libPhysXFoundation_static_64.a>
)

if(UNIX AND NOT APPLE)
    target_link_libraries(engine PRIVATE pthread dl)
endif()