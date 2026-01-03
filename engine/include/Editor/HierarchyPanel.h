//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "IPanel.h"
#include "Core/RealConfig.h"
#include "Scene/Components.h"

namespace Real::UI { class EditorPanel; }

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
        bool m_OpenLightDir = false;
        float m_SizeX = SCREEN_WIDTH / 5 + 31.0;
        float m_SizeY = SCREEN_HEIGHT;
        int m_IDcounter = 0;

    private:
        void DrawComponents(Scene* scene);
        void DrawComponent(TagComponent* comp, Scene* scene);
        void DrawComponent(TransformComponent* comp, Scene* scene);
        void DrawComponent(MeshRendererComponent* comp, Scene* scene);
        void DrawComponent(LightComponent* comp, TransformComponent* tc, Scene* scene);
        void DrawComponent(CameraComponent* comp, Scene* scene);

        void DrawCustomTextShape(const std::string& text, ImVec2 boxSize, ImVec4 color, bool textColorActive = false, ImVec4 textColor = ImVec4());
        void DrawCustomSizedDragger(float dragWidth, float& val, float speed, float v_min, float v_max, const char* format = "%.3f");
    };
}
