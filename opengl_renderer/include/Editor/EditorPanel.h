//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "ImGuizmo/ImGuizmo.h"
#include <string>
#include <unordered_map>
#include "IPanel.h"

namespace Real {

    namespace UI {
        class HierarchyPanel;
        class InspectorPanel;
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
        explicit EditorPanel(Graphics::Window* window, HierarchyPanel* hierarchyPanel, InspectorPanel* inspectorPanel);
        void BeginFrame() override;
        void Render(Scene* scene, opengl::Renderer* renderer) override;
        void EndFrame() override {}
        void Shutdown() override;

    private:
        Graphics::Window* m_Window;
        friend class HierarchyPanel;
        friend class InspectorPanel;
        HierarchyPanel* m_HierarchyPanel;
        InspectorPanel* m_InspectorPanel;
        bool openPerfProfile = false;
        ImGuizmo::OPERATION m_GizmoType = ImGuizmo::TRANSLATE;

    private:
        void Render(Scene* scene);

        void RenderMenuBar();
        void DrawPerformanceProfile();
        void UpdateInputUI();

        void InitFontStyle();
        void InitDarkTheme();

        void DrawGizmos();
        void DebugGizmos();
    };
}
