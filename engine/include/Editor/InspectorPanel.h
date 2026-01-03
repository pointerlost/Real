//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include "IPanel.h"
#include "../Core/RealConfig.h"

namespace Real::UI {
    class EditorPanel;
}

namespace Real::UI {

    class InspectorPanel final : public IPanel {
    public:
        explicit InspectorPanel();
        void BeginFrame() override;
        void Render(Scene* scene, opengl::Renderer* renderer) override;
        void EndFrame() override {}
        void Shutdown() override;

    private:
        bool m_Open = true;
        bool m_OpenRClickWindow = false;
        float m_SizeX = SCREEN_WIDTH / 5 + 31.0;
        float m_SizeY = SCREEN_HEIGHT;

    private:
        void IterateEntities(Scene* scene);
        void DrawRightClickWindow(Scene* scene);
    };
}
