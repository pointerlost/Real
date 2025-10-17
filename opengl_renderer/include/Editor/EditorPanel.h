//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include "IPanel.h"

namespace Real::Graphics {
    class Window;
}

namespace Real::UI {

    class EditorPanel final : public IPanel {
    public:
        explicit EditorPanel(Graphics::Window* window);
        void BeginFrame() override;
        void Render(Scene* scene) override;
        void Shutdown() override;

    private:
        Graphics::Window* m_Window;
        friend class InspectorPanel;
        friend class HierarchyPanel;

    private:
        void RenderMenuBar();
        void DrawPerformanceProfile();
    };
}
