//
// Created by pointerlost on 10/17/25.
//
#include "Editor/InspectorPanel.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Services.h"
#include "Editor/EditorState.h"
#include "Graphics/Config.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"


namespace Real::UI {

    InspectorPanel::InspectorPanel()
    {
    }

    void InspectorPanel::BeginFrame() {
    }

    void InspectorPanel::Render(Scene* scene, opengl::Renderer* renderer) {
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_Always);
        ImGui::Begin("Inspector", &m_Open, ImGuiWindowFlags_NoResize);

        IterateEntities(scene);

        ImGui::End();
    }

    void InspectorPanel::Shutdown() {
    }

    void InspectorPanel::IterateEntities(Scene* scene) {
        const auto& editorState = Services::GetEditorState();
        const auto& entities = scene->GetEntities();
        for (const auto &entity: entities | std::views::values) {
            const auto& tag = entity.GetComponent<TagComponent>()->Tag;
            if (ImGui::Selectable(tag.c_str())) {
                editorState->selectedEntity = entity;
            }
        }
    }
}
