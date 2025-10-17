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

    InspectorPanel::InspectorPanel(EditorPanel *panel) : m_EditorPanel(panel)
    {
    }

    void InspectorPanel::BeginFrame() {
    }

    void InspectorPanel::Render(Scene* scene) {
        PushHeaderStyleProps();
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        // MainMenuBarPanelY size = 23
        ImGui::SetNextWindowPos(ImVec2(0, 23), ImGuiCond_Always);
        ImGui::Begin("Inspector", &m_Open, ImGuiWindowFlags_NoResize);

        IterateEntities(scene);

        ImGui::End();
        EndHeaderStyleProps();
    }

    void InspectorPanel::Shutdown() {
    }

    void InspectorPanel::PushHeaderStyleProps() {
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.19, 0.07, 0.79, 1.0));
        ImGui::PushStyleColor(ImGuiCol_TitleBg,       ImVec4(0.09, 0.07, 0.09, 1.0));
        ImGui::PushStyleColor(ImGuiCol_Header,        ImVec4(0.25, 0.25, 0.25, 1.0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.02, 0.65, 0.02, 1.0));
    }

    void InspectorPanel::EndHeaderStyleProps() {
        ImGui::PopStyleColor(4);
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
