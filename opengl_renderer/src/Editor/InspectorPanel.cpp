//
// Created by pointerlost on 10/17/25.
//
#include "Editor/InspectorPanel.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Editor/EditorState.h"
#include "Graphics/Config.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
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
        if (Input::IsKeyPressed(REAL_KEY_ENTER)) m_OpenRClickWindow = !m_OpenRClickWindow;
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_Always);
        ImGui::Begin("Inspector", &m_Open, ImGuiWindowFlags_NoResize);
        ImGui::PushFont(Services::GetAssetManager()->GetFontStyle("Ubuntu-Regular-Big"));

        IterateEntities(scene);
        if (m_OpenRClickWindow)
            DrawRightClickWindow(scene);

        ImGui::PopFont();
        ImGui::End();
    }

    void InspectorPanel::Shutdown() {
    }

    void InspectorPanel::IterateEntities(Scene* scene) {
        const auto& editorState = Services::GetEditorState();
        auto& entities = scene->GetEntities();
        for (auto& entity: entities | std::views::values) {
            const auto& tag = entity.GetComponent<TagComponent>()->m_Tag;
            if (ImGui::Selectable(tag.c_str())) {
                editorState->selectedEntity = &entity;
            }
        }
    }

    void InspectorPanel::DrawRightClickWindow(Scene* scene) {
        ImGui::BeginListBox("##rightClick", ImVec2(100, 100));

        // TODO: need update!
        if (ImGui::Selectable("Create")) {
            if (ImGui::Selectable("Point Light")) {
                scene->CreateLight(std::string(), LightType::POINT);
            }
            if (ImGui::Selectable("Directional Light")) {
                scene->CreateLight(std::string(), LightType::DIRECTIONAL);
            }
            if (ImGui::Selectable("Spot Light")) {
                scene->CreateLight(std::string(), LightType::SPOT);
            }
        }

        ImGui::EndListBox();
    }
}
