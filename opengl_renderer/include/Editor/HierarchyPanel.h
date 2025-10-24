//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "IPanel.h"
#include "Graphics/Config.h"
#include "Scene/Components.h"

namespace Real::UI {
    class EditorPanel;
}

namespace Real::UI {

    class HierarchyPanel final : public IPanel {
    public:
        explicit HierarchyPanel();
        void BeginFrame() override;
        void Render(Scene* scene, opengl::Renderer* renderer) override;
        void EndFrame() override {}
        void Shutdown() override;

    private:
        bool m_Open = true;
        float m_SizeX = SCREEN_WIDTH / 5 + 31.0;
        float m_SizeY = SCREEN_HEIGHT;
        int m_IDcounter = 0;

    private:
        void DrawComponents(Scene* scene);
        void DrawComponent(TagComponent* comp);
        void DrawComponent(TransformComponent* comp);
        void DrawComponent(MaterialComponent* comp);
        void DrawComponent(MeshComponent* comp);
        void DrawComponent(LightComponent* comp);
        void DrawComponent(CameraComponent* comp);

        void DrawCustomTextShape(const std::string& text, ImVec2 boxSize, ImVec4 color, bool textColorActive = false, ImVec4 textColor = ImVec4());
        void DrawCustomSizedDragger(float dragWidth, float& val, float speed, float v_min, float v_max, const char* format = "%.3f");
    };
}
