//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Editor.h"
#include "EditorState.h"
#include "Core/IApplication.h"
#include "Core/Utils.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Input/CameraInput.h"
#include "Resource/ResourceLoader.h"

namespace Real {
    class RealTimeTimer;
    class AssetImporter;
    class MeshManager;
    class AssetManager;

    namespace core {
        class IRenderer;
    }
}

namespace Real::editor::app {

    class EditorApplication final : core::IApplication {
    public:
        EditorApplication(
            AssetManager& assetManager,
            AssetImporter& assetImporter,
            MeshManager& meshManager,
            Scene* scene
        ) noexcept;

        void Init() override;
        void Update(float dt) override;
        void Render() override;
        void Shutdown() override;

    private:
        Scope<UI::Editor> m_Editor;
        Scope<EditorState> m_State;
        Scope<CameraInput> m_CameraInput;
        Scope<ResourceLoader> m_ResourceLoader;
        Scope<graphics::debug::DebugRenderer> m_DebugRenderer;

        Scene* m_Scene = nullptr;
    };
}
