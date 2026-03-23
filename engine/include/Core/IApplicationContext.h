//
// Created by pointerlost on 3/21/26.
//
#pragma once

namespace Real {
    class ResourceLoader;
    class MeshManager;
    class AssetImporter;
    class AssetManager;
    class SceneManager;

    namespace graphics::debug {
        class DebugRenderer;
    }

    namespace ecs {
        class CameraSystem;
    }
}

namespace Real::core {

    class IApplicationContext {
    public:
        virtual ~IApplicationContext() = default;

        virtual void SetWindow(IWindow*) {}

        // Assets
        virtual void SetAssetManager  (AssetManager*)   {}
        virtual void SetAssetImporter (AssetImporter*)  {}
        virtual void SetMeshManager   (MeshManager*)    {}
        virtual void SetResourceLoader(ResourceLoader*) {}

        // Scene
        virtual void SetSceneManager(SceneManager*) {}

        // Debug
        virtual void SetDebugRenderer(graphics::debug::DebugRenderer*) {}

        // Systems
        virtual void SetCameraSystem(ecs::CameraSystem*) {}
    };
}
