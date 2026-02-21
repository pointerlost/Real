//
// Created by pointerlost on 2/16/26.
//
#include <Core/Engine/EngineCore.h>
#include "Core/IApplication.h"
#include "Core/IRenderer.h"
#include "Core/IPlatform.h"
#include "Core/IPhysicsBackend.h"
#include "Core/Window/IWindow.h"
#include "Input/Input.h"

namespace Real::core {

    EngineCore::EngineCore(CoreSystems& cs, AssetSystems& as)
        : m_Window(std::move(cs.window)),
        m_Platform(std::move(cs.platform)),
        m_Renderer(std::move(cs.renderer)),
        m_PhysicsBackend(std::move(cs.physicsBackend)),
        m_DebugRenderer(std::move(cs.debugRenderer)),
        m_Scene(std::move(cs.scene)),
        m_Systems(std::move(cs.systems)),
        m_Timer(std::move(cs.timer)),
        m_AssetManager(std::move(as.assetManager)),
        m_MeshManager(std::move(as.meshManager)),
        m_ResourceLoader(std::move(as.resourceLoader)),
        m_AssetImporter(std::move(as.assetImporter))
    {
    }

    void EngineCore::Start() {
        m_Timer->Start();
    }

    void EngineCore::RunLoop() {
        if (!m_Application)
            throw std::runtime_error("Application not set");

        m_Application->Init();

        while (!ShouldClose()) {
            StartPhase();
            UpdatePhase();
            RenderPhase(); // TODO: Thread-safe rendering
            EndPhase();
        }

        m_Application->Shutdown();
    }

    void EngineCore::Stop() {
    }

    void EngineCore::SetApplication(Scope<IApplication> application) {
        m_Application = std::move(application);
    }

    void EngineCore::StartPhase() const {
        m_Window->PollEvents();
        m_Renderer->BeginFrame();
    }

    void EngineCore::UpdatePhase() const {
        const auto dt = static_cast<float>(m_Timer->GetDelta());

        Input::Update();
        m_AssetImporter->Update();
        m_AssetManager->Update();
        m_Systems->Update(m_Scene->GetRegistry(), dt);

        m_Application->Update(dt);

        m_Scene->Update(m_Renderer.get());
        Services::GetDebugRenderer()->Update();
    }

    void EngineCore::RenderPhase() const {
    }

    void EngineCore::EndPhase() {
    }

    bool EngineCore::ShouldClose() const {
        return m_ShouldStop || m_Window->ShouldClose();
    }
}
