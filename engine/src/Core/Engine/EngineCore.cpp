//
// Created by pointerlost on 2/16/26.
//
#include <Core/Engine/EngineCore.h>
#include "Scene/Scene.h"
#include "Graphics/Shader.h"
#include "Input/Input.h"
#include <stdexcept>

#include "Core/Logger.h"

namespace Real::core {

    EngineCore::EngineCore(Scope<CoreSystems> cs, Scope<AssetSystems> as, Scope<IApplication> application)
        : m_Application(std::move(application)),
          m_Core(std::move(cs)),
          m_Assets(std::move(as))
    {
    }

    void EngineCore::Start() {
        Timer().Start();

        m_Core->systems->Init();

        // systems attach to the active scene before the loop begins
        Systems().OnSceneAttach(
            ActiveScene().GetRegistry(),
            ActiveScene().GetEvents()
        );

        m_Assets->meshManager->InitResources();

        m_Core->renderer->Init();
        Info("Shit");

        m_Assets->resourceLoader->Load();
        Info("Shit");

        m_Core->debugRenderer->Init();

        m_Assets->assetManager->UploadTexturesToGPU();
        Info("Shit");
    }

    void EngineCore::RunLoop() {
        if (!m_Application)
            throw std::runtime_error("Application not set");

        InitApplication();
        InitRendererBackend();

        while (!ShouldClose()) {
            StartPhase();
            UpdatePhase();
            RenderPhase(); // TODO: Thread-safe rendering
            EndPhase();
        }

        ShutdownApplication();
        ShutdownRendererBackend();
    }

    void EngineCore::Stop() {
    }

    Scene& EngineCore::ActiveScene() const noexcept {
        auto* scene = Scenes().GetActiveScene();
        assert(scene && "No active scene");
        return *scene;
    }

    void EngineCore::StartPhase() const {
        Window().PollEvents();
        Renderer().BeginFrame();
    }

    void EngineCore::UpdatePhase() const {
        const auto dt = static_cast<f32>(Timer().GetDelta());

        Input::Update();
        Importer().Update();
        Assets().Update();
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
    }

    void EngineCore::ShutdownApplication() {
        m_Application->Shutdown();
    }

    void EngineCore::InitRendererBackend() {
        Renderer().Init();
    }

    void EngineCore::ShutdownRendererBackend() {
    }

    bool EngineCore::ShouldClose() {
        return m_ShouldStop || Window().ShouldClose();
    }
}
