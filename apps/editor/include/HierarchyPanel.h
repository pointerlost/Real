//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include "IPanel.h"
#include "Core/RealConfig.h"
#include "Common/RealTypes.h"

namespace Real::UI {
    class Editor;
}

namespace Real::UI {

    class HierarchyPanel final : public IPanel {
    public:
        explicit HierarchyPanel();
        void OnImGuiRender() override;
        void OnSceneAttached(Scene *scene) override;

    private:
        Scene* m_Scene = nullptr;
        bool m_Open = true;
        bool m_OpenRClickWindow = false;
        f32 m_SizeX = SCREEN_WIDTH / 5 + 31.0;
        f32 m_SizeY = SCREEN_HEIGHT;

    private:
        void IterateEntities();
        void DrawRightClickWindow();
    };
}
