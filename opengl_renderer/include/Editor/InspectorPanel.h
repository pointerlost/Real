//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include "IPanel.h"
#include "Graphics/Config.h"

namespace Real::UI {
    class EditorPanel;
}

namespace Real::UI {

    class InspectorPanel final : public IPanel {
    public:
        explicit InspectorPanel(EditorPanel* panel);
        void BeginFrame() override;
        void Render(Scene* scene) override;
        void Shutdown() override;

    private:
        EditorPanel* m_EditorPanel;

        bool m_Open = true;
        float m_SizeX = SCREEN_WIDTH / 5;
        float m_SizeY = SCREEN_HEIGHT;

    private:
        void PushHeaderStyleProps();
        void EndHeaderStyleProps();
        void IterateEntities(Scene* scene);
    };
}
