//
// Created by pointerlost on 10/17/25.
//
#pragma once
#include <imgui.h>
#include "IPanel.h"
#include "Core/RealConfig.h"
#include "Scene/Components.h"

namespace Real {
    class Entity;
}

namespace Real::UI { class EditorPanel; }

namespace Real::UI {

    class InspectorPanel final : public IPanel {
    public:
        explicit InspectorPanel();
        void BeginFrame() override;
        void Update() override {}
        void Render(Scene* scene, opengl::OpenGLRenderer* renderer) override;
        void EndFrame() override {}
        void Shutdown() override;

    private:
        bool m_Open = true;
        f32 m_SizeX = SCREEN_WIDTH / 5 + 31.0;
        f32 m_SizeY = SCREEN_HEIGHT;
        int m_IDcounter = 0;

    private:
        void DrawComponents(Scene* scene);
        void DrawComponent(TagComponent* comp, Scene* scene);
        void DrawComponent(TransformComponent* comp, Scene* scene);
        void DrawComponent(const MeshRendererComponent* comp, Scene* scene);
        void DrawComponent(Entity& entity, ColliderComponent *comp, Scene *scene);
        void DrawComponent(Entity& entity, RigidbodyComponent *comp, Scene *scene);
        void DrawComponent(LightComponent* comp, TransformComponent* tc, Scene* scene);
        void DrawComponent(CameraComponent* comp, Scene* scene);

        void DrawCustomTextShape(const String& text, ImVec2 boxSize, ImVec4 color, bool textColorActive = false, ImVec4 textColor = ImVec4());
        void DrawCustomSizedDragger(f32 dragWidth, f32& val, f32 speed, f32 v_min, f32 v_max, const char* format = "%.3f");
    };
}
