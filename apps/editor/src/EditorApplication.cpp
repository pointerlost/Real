//
// Created by pointerlost on 2/19/26.
//
#include "../apps/editor/include/EditorApplication.h"

#include "Core/Services.h"
#include "Core/Engine/EngineCore.h"
#include "Scene/Components.h"

namespace Real::editor::app {

    EditorApplication::EditorApplication(
        AssetManager& assetManager,
        AssetImporter& assetImporter,
        MeshManager& meshManager,
        Scene* scene) noexcept
    {
        m_Scene = scene;
        Services::SetAssetManager(&assetManager);
        Services::SetAssetImporter(&assetImporter);
        Services::SetMeshManager(&meshManager);

        m_State = CreateScope<EditorState>();
        Services::SetEditorState(m_State.get());
    }

    void EditorApplication::Init() {
        m_State->editorCamera = &m_Scene->CreateEntity("Editor Camera");
        (void)m_State->editorCamera->AddComponent<CameraComponent>();
        (void)m_State->editorCamera->AddComponent<MovementComponent>();
        m_State->editorCamera->GetComponentUnchecked<TransformComponent>().transform.SetPosition(math::Vec3(0.0, 2.0, 5.0));

        if (m_State->editorCamera) {
            m_CameraInput = CreateScope<CameraInput>(m_State->editorCamera);
        } else {
            Warn("There is no camera in editor state!!!");
        }

        m_ResourceLoader = CreateScope<ResourceLoader>(m_Renderer->GetRenderContext());
        m_ResourceLoader->Load();

        m_DebugRenderer = CreateScope<graphics::debug::DebugRenderer>();
        m_DebugRenderer->Init();

        Services::SetDebugRenderer(m_DebugRenderer.get());
    }

    void EditorApplication::Update(float dt) {
        m_CameraInput->Update();
        m_Editor->Update(dt);
        m_CameraInput->Update();
    }

    void EditorApplication::Render() {
        m_Editor->RenderUI();
    }

    void EditorApplication::Shutdown() {
        m_CameraInput.reset();
        m_DebugRenderer.reset();
        m_Timer.reset();
        m_State.reset();
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