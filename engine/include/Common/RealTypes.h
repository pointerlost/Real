//
// Created by pointerlost on 12/6/25.
//
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <Core/UUID.h>

#include "Core/Window/WindowConfig.h"

namespace Real {
    class RealTimeTimer;
    class AssetImporter;
    class ResourceLoader;
    class MeshManager;
    class AssetManager;
    class ModelLoader;
    class Scene;

    namespace physics {
        class IPhysicsBackend;
    }

    namespace graphics::debug {
        class DebugRenderer;
    }

    namespace core {
        class SystemManager;
        class IRenderer;
        class IPlatform;
        class IWindow;
        class IRenderDevice;
    }
}

namespace Real {

    struct Vertex;
    using i8  = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    template<typename T>
    using Vector = std::vector<T>;

    using String = std::string;

    constexpr u32 REAL_MAGIC = 0x4C414552; // Little endian

    enum struct WindowType {
        glfw,
        sdl,
    };

    enum struct RendererType {
        OpenGL,
        Vulkan
    };

    enum struct API {
        OpenGL,
        Vulkan
    };

    struct OpenGLConfig {
        int major = 4;
        int minor = 5;

        bool coreProfile  = true;
        bool debugContext = false;
        bool depthTesting = true;
        bool vsync = true;
    };

    struct VkConfig {
    };

    struct RendererConfig {
        RendererType type = RendererType::OpenGL;

        OpenGLConfig opengl;
        VkConfig vulkan;
    };

    enum struct PhysicsBackendType {
        PhysX,
        Bullet,
    };

    struct EngineConfig {
        bool editorMode = true;
        core::WindowConfig windowConfig;
        RendererConfig rendererConfig;
        API apiType;
        RendererType rendererType;
        PhysicsBackendType physicsBackendType;
    };

    struct CoreSystems {
        std::unique_ptr<core::IWindow> window;
        std::unique_ptr<core::IPlatform> platform;
        std::unique_ptr<core::IRenderer> renderer;
        std::unique_ptr<physics::IPhysicsBackend> physicsBackend;
        std::unique_ptr<graphics::debug::DebugRenderer> debugRenderer;
        std::unique_ptr<Scene> scene;
        std::unique_ptr<core::SystemManager> systems;
        std::unique_ptr<RealTimeTimer> timer;
    };

    struct AssetSystems {
        std::unique_ptr<AssetManager> assetManager;
        std::unique_ptr<MeshManager> meshManager;
        std::unique_ptr<ResourceLoader> resourceLoader;
        std::unique_ptr<AssetImporter> assetImporter;
    };

    struct TextureData {
        void* data = nullptr;
        int channelCount = 0;
        int width = 0;
        int height = 0;
        int dataSize = 0;
        int format = {};
        int internalFormat = {};
    };

    struct FileInfo {
        String name; // Full name
        String stem; // Name without extension
        String path; // Full path
        String ext;  // Extension
    };

#pragma pack(push, 1)
    struct ModelBinaryHeader {
        u32 magic = REAL_MAGIC; // Real Magic number
        u32 version = 1;
        u32 meshCount{};
        u64 uuid{};
    };

    struct MeshBinaryHeader {
        u32 magic = REAL_MAGIC;
        u32 version = 1;

        u64 uuid{};
        u64 materialUUID{};

        u64 vertexCount{};
        u64 indexCount{};
        u64 vertexOffset{};
        u64 indexOffset{};
    };
#pragma pack(pop)

    struct MeshLoadResult {
        MeshBinaryHeader header;
        Vector<Vertex> vertices;
        Vector<u32> indices;
    };

    struct MeshAsset {
        UUID meshUUID;
        // TODO: Need transform for per mesh!

        u64 vertexCount;
        u64 indexCount;
        u64 vertexOffset;
        u64 indexOffset;
    };

    struct RenderableData {
        const MeshAsset* mesh;
        UUID materialUUID{};
        // TODO: Need transform for per mesh!
    };

}
