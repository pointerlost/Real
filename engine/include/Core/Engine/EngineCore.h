//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/Utils.h"
#include "Common/RealTypes.h"
#include "Core/AssetImporter.h"
#include "Resource/ResourceLoader.h"
#include "Core/AssetManager.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Timer/Timer.h"
#include "Scene/Systems/SystemsManager.h"
#include "Scene/Scene.h"

namespace Real {
    namespace core {
        class IRenderer;
        class IPlatform;
        class IWindow;
        class IApplication;
    }

    namespace physics {
        class IPhysicsBackend;
    }
}

namespace Real::core {

    class EngineCore {
    public:
        explicit EngineCore(CoreSystems& cs, AssetSystems& as);

        void Start();    // start loop (bootstrap should have done Init on subsystems)
        void RunLoop();  // orchestrates per-frame calls (poll, update, upload, render, swap)
        void Stop();     // request stop

        void SetApplication(Scope<IApplication> application);

    private:
        Scope<IApplication> m_Application;
        Scope<IWindow> m_Window;
        Scope<IPlatform> m_Platform;
        Scope<IRenderer> m_Renderer;
        Scope<physics::IPhysicsBackend> m_PhysicsBackend;
        Scope<graphics::debug::DebugRenderer> m_DebugRenderer;
        Scope<Scene> m_Scene;
        Scope<SystemManager> m_Systems;
        Scope<RealTimeTimer> m_Timer;

        Scope<AssetManager> m_AssetManager;
        Scope<MeshManager> m_MeshManager;
        Scope<ResourceLoader> m_ResourceLoader;
        Scope<AssetImporter> m_AssetImporter;

        EngineConfig m_EngineConfig;

        bool m_ShouldStop{false};

    private:
        void StartPhase() const;
        void UpdatePhase() const;
        void RenderPhase() const;
        void EndPhase();

        [[nodiscard]] bool ShouldClose() const;
    };
}