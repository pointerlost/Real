//
// Created by pointerlost on 10/17/25.
//
#include "EditorApplication/HierarchyPanel.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "../include/EditorState.h"
#include "../../../engine/include/Core/RealConfig.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"


namespace Real::UI {

    HierarchyPanel::HierarchyPanel()
    {
    }

    void HierarchyPanel::OnImGuiRender() {
        if (Input::IsKeyPressed(REAL_KEY_ENTER))
            m_OpenRClickWindow = !m_OpenRClickWindow;

        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_Always);
        ImGui::Begin("Inspector", &m_Open, ImGuiWindowFlags_NoResize);
        ImGui::PushFont(Services::GetAssetManager()->GetFontStyle("Ubuntu-Regular-Big"));

        IterateEntities();
        if (m_OpenRClickWindow)
            DrawRightClickWindow();

        ImGui::PopFont();
        ImGui::End();
    }

    void HierarchyPanel::OnSceneAttached(Scene *scene) {
        m_Scene = scene;
    }

    void HierarchyPanel::IterateEntities() {
        const auto& editorState = Services::GetEditorState();
        auto& entities = m_Scene->GetEntities();
        for (auto& entity: entities | std::views::values) {
            const auto& tag = entity.GetComponent<TagComponent>().m_Tag;
            if (ImGui::Selectable(tag.c_str())) {
                editorState->selectedEntity = &entity;
            }
        }
    }

    void HierarchyPanel::DrawRightClickWindow() {
        ImGui::BeginListBox("##rightClick", ImVec2(100, 100));

        // TODO: need update!
        if (ImGui::Selectable("Create")) {
            if (ImGui::Selectable("Point Light")) {
                m_Scene->CreateLight(String(), Light::Mode::POINT);
            }
            if (ImGui::Selectable("Directional Light")) {
                m_Scene->CreateLight(String(), Light::Mode::DIRECTIONAL);
            }
            if (ImGui::Selectable("Spot Light")) {
                m_Scene->CreateLight(String(), Light::Mode::SPOT);
            }
        }

        ImGui::EndListBox();
    }
}
