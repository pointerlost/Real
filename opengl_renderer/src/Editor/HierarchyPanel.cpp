//
// Created by pointerlost on 10/17/25.
//
#include "Editor/HierarchyPanel.h"
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Services.h"
#include "Editor/EditorState.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Real::UI {

    HierarchyPanel::HierarchyPanel(EditorPanel *panel) : m_EditorPanel(panel)
    {
    }

    void HierarchyPanel::BeginFrame() {
    }

    void HierarchyPanel::Render(Scene* scene) {
        PushHeaderStyleProps();
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        // MainMenuBarPanelY size = 23
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - m_SizeX, 23), ImGuiCond_Always);
        ImGui::Begin("Hierarchy", &m_Open, ImGuiWindowFlags_NoResize);

        DrawComponents(scene);

        ImGui::End();
        EndHeaderStyleProps();
    }

    void HierarchyPanel::Shutdown() {
    }

    void HierarchyPanel::DrawComponents(Scene* scene) {
        const auto& editorState = Services::GetEditorState();
        const auto& entity = editorState->selectedEntity;
        if (!entity) return;

        if (entity.HasComponent<TagComponent>()) {
            DrawComponent(entity.GetComponent<TagComponent>());
        }
        if (entity.HasComponent<TransformComponent>()) {
            DrawComponent(entity.GetComponent<TransformComponent>());
        }
        if (entity.HasComponent<MaterialComponent>()) {
            DrawComponent(entity.GetComponent<MaterialComponent>());
        }
        if (entity.HasComponent<MeshComponent>()) {
            DrawComponent(entity.GetComponent<MeshComponent>());
        }
        if (entity.HasComponent<LightComponent>()) {
            DrawComponent(entity.GetComponent<LightComponent>());
        }
        if (entity.HasComponent<CameraComponent>()) {
            DrawComponent(entity.GetComponent<CameraComponent>());
        }
    }

    void HierarchyPanel::DrawComponent(TagComponent *comp) {
        if (ImGui::CollapsingHeader("Tag Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(TransformComponent *comp) {
        if (ImGui::CollapsingHeader("Transform Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(MaterialComponent *comp) {
        if (ImGui::CollapsingHeader("Material Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(MeshComponent *comp) {
        if (ImGui::CollapsingHeader("Mesh Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(LightComponent *comp) {
        if (ImGui::CollapsingHeader("Light Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(CameraComponent *comp) {
        auto& camera = comp->m_Camera;
        auto near = camera.GetNear();
        auto far = camera.GetFar();
        auto fov = camera.GetFOV();
        auto aspect = camera.GetAspect();
        if (ImGui::CollapsingHeader("Camera Component")) {
            // TODO: redesign drag floats with new values
            if (ImGui::DragFloat("Near Plane", &near, 0.01, 0.001, 500.0)) {
                camera.SetNear(near);
            }
            if (ImGui::DragFloat("Far Plane", &far, 0.05, 0.001, 100000.0)) {
                camera.SetFar(far);
            }
            if (ImGui::DragFloat("FOV", &fov, 0.01, 0.001, 500.0)) { // Change the values
                camera.SetFOV(fov);
            }
            if (ImGui::DragFloat("Aspect", &aspect, 0.01, 0.001, 500.0)) { // Change the values
                camera.SetFar(aspect);
            }
        }
    }

    void HierarchyPanel::PushHeaderStyleProps() {
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.19, 0.07, 0.79, 1.0));
        ImGui::PushStyleColor(ImGuiCol_TitleBg,       ImVec4(0.09, 0.07, 0.09, 1.0));
        ImGui::PushStyleColor(ImGuiCol_Header,        ImVec4(0.25, 0.25, 0.25, 1.0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.02, 0.65, 0.02, 1.0));
    }

    void HierarchyPanel::EndHeaderStyleProps() {
        ImGui::PopStyleColor(4);
    }
}
