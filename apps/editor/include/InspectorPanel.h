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
    namespace UI {
        class Editor;
    }
}

namespace Real::UI {

    class InspectorPanel final : public IPanel {
    public:
        InspectorPanel();

        void OnImGuiRender() override;
        void OnSceneAttached(Scene *scene) override;

    private:
        Scene* m_Scene = nullptr;

        bool   m_Open      = true;
        int    m_IDCounter = 0;
        f32    m_SizeX     = SCREEN_WIDTH / 5 + 31.0; // remove hardcoded shit
        f32    m_SizeY     = SCREEN_HEIGHT;

    private:
        void DrawComponents();
        void DrawComponent(TagComponent* comp);
        void DrawComponent(TransformComponent* comp);
        void DrawComponent(const MeshRendererComponent* comp);
        void DrawComponent(Entity& entity, ColliderComponent *comp) const;
        void DrawComponent(Entity& entity, RigidbodyComponent *comp) const;
        void DrawComponent(LightComponent* comp, TransformComponent* tc);
        void DrawComponent(CameraComponent* comp);

        void DrawCustomTextShape(const String& text, ImVec2 boxSize, ImVec4 color, bool textColorActive = false, ImVec4 textColor = ImVec4());
        void DrawCustomSizedDragger(f32 dragWidth, f32& val, f32 speed, f32 v_min, f32 v_max, const char* format = "%.3f");
    };
}
