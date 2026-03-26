//
// Created by pointerlost on 3/21/26.
//
#pragma once

namespace Real::assets {
    class ResourceManager;
}

namespace Real {
    class SceneManager;

    namespace assets {
        class MeshManager;
        class AssetImporter;
        class AssetManager;
    }

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
        virtual void SetAssetManager  (assets::AssetManager*)    {}
        virtual void SetAssetImporter (assets::AssetImporter*)   {}
        virtual void SetMeshManager   (assets::MeshManager*)     {}
        virtual void SetResourceLoader(assets::ResourceManager*) {}

        // Scene
        virtual void SetSceneManager(SceneManager*) {}

        // Debug
        virtual void SetDebugRenderer(graphics::debug::DebugRenderer*) {}

        // Systems
        virtual void SetCameraSystem(ecs::CameraSystem*) {}
    };
}
