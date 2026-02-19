//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>

#include "Common/RealTypes.h"
#include "ImGuizmo/ImGuizmo.h"
#include "IEditor.h"
#include "Core/RealConfig.h"
#include "Core/Utils.h"

namespace Real::UI {
    class IPanel;
}

namespace Real {

    namespace UI {
        class InspectorPanel;
        class HierarchyPanel;
    }

    namespace opengl { class OpenGLRenderer; }
    namespace core   { class IWindow; }
}

namespace Real::UI {

    class Editor final : public core::IEditor {
    public:
        explicit Editor(core::IWindow* window);

        void Init() override;
        void BeginFrame(float dt) override;
        void Update(float dt) override;
        void RenderUI() override;
        void EndFrame() override;

        void OnAttachScene(Scene *scene) override;
        void Shutdown() override;

    private:
        Scene* m_Scene = nullptr;
        core::IWindow* m_Window = nullptr;
        Vector<Scope<IPanel>> m_Panels;
        bool m_OpenPerformanceProfile = false;
        ImGuizmo::OPERATION m_GizmoType = ImGuizmo::TRANSLATE;
        // User intention
        bool m_EditEntityTransform = true;

        // Screen height can wrong for editor-time, because of main menu panel has some height
        ImVec2 m_SceneWindowSize = ImVec2(SCREEN_WIDTH - (SCREEN_WIDTH / 5 + 31.0) * 2, SCREEN_HEIGHT);

    private:
        void Render();

        void RenderMenuBar();
        void RenderFileBar();
        void RenderEditBar();
        void RenderViewBar();
        void RenderDebugBar();
        void RenderHelpBar();

        void DrawPerformanceProfile();
        void UpdateInputUI();

        void InitFontStyle();
        void InitDarkTheme();

        void RenderSceneGizmos();
        void UpdateGizmoLogic();
        void DebugGizmos();
    };
}
