//
// Created by pointerlost on 2/19/26.
//
#include "EditorApplication.h"
#include "IPanel.h"
#include "UIResources.h"
#include "Core/Engine/EngineCore.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Scene/Components.h"

namespace Real::app::editor {

    EditorApplication::EditorApplication(EditorContext ctx) noexcept
        : m_Ctx(std::move(ctx)), m_Editor(CreateScope<UI::Editor>()), m_UIResources(CreateScope<Real::editor::UIResources>())
    {
        m_State = CreateScope<EditorState>();
        Services::SetEditorState(m_State.get());
    }

    void EditorApplication::Init() {
        auto* scene = m_Ctx.sceneManager->GetActiveScene();

        m_State->editorCamera = &scene->CreateEntity("Editor Camera");
        m_State->editorCamera->AddComponent<CameraComponent>();
        m_State->editorCamera->AddComponent<MovementComponent>();
        m_State->editorCamera->GetComponentUnchecked<TransformComponent>().transform.SetLocalPosition(math::Vec3(0.0, 2.0, 5.0));

        if (m_State->editorCamera) {
            m_CameraInput = CreateScope<CameraInput>(m_State->editorCamera);
        } else {
            Warn("There is no camera in editor state!!!");
        }

        scene->SetActiveCamera(m_State->editorCamera);

        m_Editor->OnAttachScene(scene);

        m_Editor->Init(m_Ctx.window);

        m_UIResources->InitResources();
    }

    void EditorApplication::Update(float dt) {
        m_CameraInput->Update();
        m_Editor->Update(dt);
        m_CameraInput->Update();
        m_Ctx.debugRenderer->Update();
    }

    void EditorApplication::Render() {
        m_Editor->RenderUI();
    }

    void EditorApplication::Shutdown() {
    }
}

/*
    void EditorApplication::BeginFrame() {
        m_Editor->BeginFrame(m_Timer.GetDelta());
    }
    void EditorApplication::EndFrame() {
        m_Editor->EndFrame();
    }
*/