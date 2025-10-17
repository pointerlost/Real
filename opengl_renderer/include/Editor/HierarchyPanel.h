//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include "IPanel.h"
#include "Graphics/Config.h"
#include "Scene/Components.h"

namespace Real::UI {
    class EditorPanel;
}

namespace Real::UI {

    class HierarchyPanel final : public IPanel {
    public:
        explicit HierarchyPanel(EditorPanel* panel);
        void BeginFrame() override;
        void Render(Scene* scene) override;
        void Shutdown() override;

    private:
        EditorPanel* m_EditorPanel;

        bool m_Open = true;
        float m_SizeX = SCREEN_WIDTH / 5;
        float m_SizeY = SCREEN_HEIGHT;

    private:
        void DrawComponents(Scene* scene);
        void DrawComponent(TagComponent* comp);
        void DrawComponent(TransformComponent* comp);
        void DrawComponent(MaterialComponent* comp);
        void DrawComponent(MeshComponent* comp);
        void DrawComponent(LightComponent* comp);
        void DrawComponent(CameraComponent* comp);

        void PushHeaderStyleProps();
        void EndHeaderStyleProps();
    };
}
