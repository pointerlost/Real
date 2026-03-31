//
// Created by pointerlost on 2/16/26.
//
#include <Core/Engine/EngineCore.h>
#include "Scene/Scene.h"
#include "Graphics/Shader.h"
#include "Input/Input.h"
#include <stdexcept>
#include "Graphics/RenderContext.h"
#include "Assets/AssetTypes.h"
#include "Core/Logger.h"

namespace Real::core {

    EngineCore::EngineCore(Scope<CoreSystems> cs, Scope<AssetSystems> as, Scope<IApplication> application)
        : m_Application(std::move(application)),
          m_Core(std::move(cs)),
          m_Assets(std::move(as))
    {
        // Init frame config
        m_FrameConfig.clearColor = { 0.07f, 0.07f, 0.07f, 1.0f };
        m_FrameConfig.clearFlags = { graphics::ClearFlags::Color | graphics::ClearFlags::Depth };
    }

    void EngineCore::Start() {
        Timer().Start();
        Info("Engine Timer initialized!");

        m_Core->systems->Init();
        Info("Engine CoreSystems initialized!");

        // systems attach to the active scene before the loop begins
        Systems().OnSceneAttach(
            ActiveScene().GetRegistry(),
            ActiveScene().GetEvents()
        );
        Info("Systems attached to the active scene successfully!!");

        m_Assets->resourceManager->Load(m_Core.get());
        Info("ResourceManager initialized successfully!!");

        m_Core->debugRenderer->Init();
        Info("DebugRenderer initialized successfully!!");

        if (!m_Application)
            throw std::runtime_error("Application not set");

        InitApplication();
        InitRendererBackend();
    }

    void EngineCore::RunLoop() {

        while (!ShouldClose()) {
            StartPhase();
            UpdatePhase();
            RenderPhase(); // TODO: Thread-safe rendering
            EndPhase();
        }
    }

    void EngineCore::Stop() {
        ShutdownApplication();
        ShutdownRendererBackend();
    }

    Scene& EngineCore::ActiveScene() const noexcept {
        auto* scene = Scenes().GetActiveScene();
        assert(scene && "No active scene");
        return *scene;
    }

    void EngineCore::StartPhase() const {
        Window().PollEvents();
        Renderer().BeginFrame(m_FrameConfig);
    }

    void EngineCore::UpdatePhase() const {
        const auto dt = static_cast<f32>(Timer().GetDelta());

        Input::Update();
        m_Assets->assetImporter->Update();
        Systems().Update(ActiveScene().GetRegistry(), dt);

        m_Application->Update(dt);

        ActiveScene().Update(&Renderer());
    }

    void EngineCore::RenderPhase() const {
        Renderer().Render(&ActiveScene(), ActiveScene().GetActiveCamera());
    }

    void EngineCore::EndPhase() {
        Renderer().EndFrame();
    }

    void EngineCore::InitApplication() {
        m_Application->Init();
        Info("Application initialized successfully!");
    }

    void EngineCore::ShutdownApplication() {
        m_Application->Shutdown();
    }

    void EngineCore::InitRendererBackend() {
        Renderer().Init();
        Info("Application initialized successfully!");
    }

    void EngineCore::ShutdownRendererBackend() {
    }

    bool EngineCore::ShouldClose() {
        return m_ShouldStop || Window().ShouldClose();
    }
}
