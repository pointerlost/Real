//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "ImGuizmo/ImGuizmo.h"
#include <string>
#include <unordered_map>
#include "IPanel.h"
#include "Core/RealConfig.h"

namespace Real {

    namespace UI {
        class InspectorPanel;
        class HierarchyPanel;
    }

    namespace opengl {
        class Renderer;
    }

    namespace Graphics {
        class Window;
    }
}

namespace Real::UI {

    class EditorPanel final : public IPanel {
    public:
        explicit EditorPanel(Graphics::Window* window, InspectorPanel* hierarchyPanel, HierarchyPanel* inspectorPanel);
        void BeginFrame() override;
        void Update() override;
        void Render(Scene* scene, opengl::Renderer* renderer) override;
        void EndFrame() override {}
        void Shutdown() override;

    private:
        Graphics::Window* m_Window;
        friend class InspectorPanel;
        friend class HierarchyPanel;
        InspectorPanel* m_HierarchyPanel;
        HierarchyPanel* m_InspectorPanel;
        bool openPerfProfile = false;
        ImGuizmo::OPERATION m_GizmoType = ImGuizmo::TRANSLATE;
        // User intention
        bool m_EditEntityTransform = true;

        // Screen height can wrong for editor-time, because of main menu panel has some height
        ImVec2 m_SceneWindowSize = ImVec2(SCREEN_WIDTH - (SCREEN_WIDTH / 5 + 31.0) * 2, SCREEN_HEIGHT);

    private:
        void Render(Scene* scene);

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
