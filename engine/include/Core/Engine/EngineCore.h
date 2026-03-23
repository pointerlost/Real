//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/IApplication.h"
#include "RHI/IRenderer.h"
#include "Core/IPlatform.h"
#include "Window/IWindow.h"
#include "Core/IPhysicsBackend.h"
#include "Assets/AssetImporter.h"
#include "Assets/AssetManager.h"
#include "Assets/ResourceLoader.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Scene/SceneManager.h"
#include "Timer/Timer.h"
#include "Scene/Systems/SystemsManager.h"


namespace Real::ecs { class CameraSystem; }
namespace Real { class Scene; }

namespace Real::core {

    struct CoreSystems {
        std::unique_ptr<IWindow> window;
        std::unique_ptr<IPlatform> platform;
        std::unique_ptr<IRenderer> renderer;
        std::unique_ptr<IPhysicsBackend> physicsBackend;
        std::unique_ptr<graphics::debug::DebugRenderer> debugRenderer;
        std::unique_ptr<SceneManager> sceneManager;
        std::unique_ptr<SystemManager> systems;
        std::unique_ptr<RealTimeTimer> timer;
    };

    struct AssetSystems {
        std::unique_ptr<AssetManager> assetManager;
        std::unique_ptr<MeshManager> meshManager;
        std::unique_ptr<AssetImporter> assetImporter;
        std::unique_ptr<ResourceLoader> resourceLoader;
    };


    class EngineCore {
    public:
        explicit EngineCore(Scope<CoreSystems> cs, Scope<AssetSystems> as, Scope<IApplication> application);

        void Start();   // start loop (bootstrap should have done Init on subsystems)
        void RunLoop(); // orchestrates per-frame calls (poll, update, upload, render, swap)
        void Stop();    // request stop

        // Core
        [[nodiscard]] IWindow&          Window()   const noexcept { return *m_Core->window; }
        [[nodiscard]] IPlatform&        Platform() const noexcept { return *m_Core->platform; }
        [[nodiscard]] IRenderer&        Renderer() const noexcept { return *m_Core->renderer; }
        [[nodiscard]] IPhysicsBackend&  Physics()  const noexcept { return *m_Core->physicsBackend; }
        [[nodiscard]] graphics::debug::DebugRenderer& Debug() const noexcept { return *m_Core->debugRenderer; }
        [[nodiscard]] SceneManager&     Scenes()   const noexcept { return *m_Core->sceneManager; }
        [[nodiscard]] SystemManager&    Systems()  const noexcept { return *m_Core->systems; }
        [[nodiscard]] RealTimeTimer&    Timer()    const noexcept { return *m_Core->timer; }

        // Assets
        [[nodiscard]] AssetManager&   Assets()    const noexcept { return *m_Assets->assetManager; }
        [[nodiscard]] MeshManager&    Meshes()    const noexcept { return *m_Assets->meshManager; }
        [[nodiscard]] ResourceLoader& Resources() const noexcept { return *m_Assets->resourceLoader; }
        [[nodiscard]] AssetImporter&  Importer()  const noexcept { return *m_Assets->assetImporter; }

        // Active scene shortcut
        [[nodiscard]] Scene& ActiveScene() const noexcept;

    private:
        Scope<IApplication> m_Application;
        Scope<CoreSystems>  m_Core;
        Scope<AssetSystems> m_Assets;

        ecs::CameraSystem* m_CameraSystem = nullptr;

        bool m_ShouldStop{false};

    private:
        void StartPhase()  const;
        void UpdatePhase() const;
        void RenderPhase() const;
        void EndPhase(); // non-const — mutates m_ShouldStop

        void InitApplication();
        void ShutdownApplication();
        void InitRendererBackend();
        void ShutdownRendererBackend();

        [[nodiscard]] bool ShouldClose(); // non-const — mutates m_ShouldStop
    };
}