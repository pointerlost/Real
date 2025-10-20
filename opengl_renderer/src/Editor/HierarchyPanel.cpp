//
// Created by pointerlost on 10/17/25.
//
#include "Editor/HierarchyPanel.h"
#include <imgui.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Services.h"
#include "Editor/EditorPanel.h"
#include "Editor/EditorState.h"
#include "Graphics/Material.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Real::UI {

    HierarchyPanel::HierarchyPanel()
    {
    }

    void HierarchyPanel::BeginFrame() {
    }

    void HierarchyPanel::Render(Scene* scene, opengl::Renderer* renderer) {
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - m_SizeX, 25), ImGuiCond_Always);
        ImGui::Begin("Hierarchy", &m_Open, ImGuiWindowFlags_NoResize);

        DrawComponents(scene);

        ImGui::End();
    }

    void HierarchyPanel::Shutdown() {
    }

    void HierarchyPanel::DrawComponents(Scene* scene) {
        const auto& editorState = Services::GetEditorState();
        const auto& entity = editorState->selectedEntity;
        if (!entity) return;

        // ImGui::PushFont(m_EditorPanel->GetFontStyle("Ubuntu-Bold"));
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
        // ImGui::PopFont();
        // Reset
        m_IDcounter = 0;
    }

    void HierarchyPanel::DrawComponent(TagComponent *comp) {
        if (ImGui::CollapsingHeader("Tag Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(TransformComponent *comp) {
        auto& transform = comp->m_Transform;
        auto& position = transform.GetTranslate();
        auto rotate = transform.GetRotationEuler();
        auto& scale = transform.GetScale();

        constexpr auto textboxSize = ImVec2(25.0, 30.0);
        constexpr auto textSize = ImVec2(70.0, 30.0);
        constexpr float dragCount = 3.0;
        const auto dragSize = static_cast<float>((m_SizeX - 3.0 * textboxSize.x - textSize.x) / dragCount - 20.0);
        if (ImGui::CollapsingHeader("Transform Component")) {
            // Translate
            {
                DrawCustomTextShape("Position", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.x, 0.1, -360.0, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.y, 0.1, -360.0, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.z, 0.1, -360.0, 360.0);
            }

            // Rotate
            {
                DrawCustomTextShape("Rotation", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.x, 0.1, -360.0, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.y, 0.1, -360.0, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.z, 0.1, -360.0, 360.0);

                // rotate is a copy-value, so set it
                transform.SetRotationEuler(rotate);
            }

            // Scale
            {
                DrawCustomTextShape("Scale", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.x, 0.1, 0.01, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.y, 0.1, 0.01, 360.0);
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.z, 0.1, 0.01, 360.0);
            }
        }
    }

    void HierarchyPanel::DrawComponent(MaterialComponent *comp) {
        const auto& material = comp->m_Instance->m_Base;
        auto& baseColor = material->BaseColor;
        auto& emissive = material->emissive;
        auto& metallic = material->Metallic;
        auto& roughness = material->Roughness;
        constexpr auto textboxSize = ImVec2(22.0, 30.0);
        constexpr auto textSize = ImVec2(45.0, 30.0);
        constexpr float dragCount = 4.0;
        const auto dragSize = static_cast<float>((m_SizeX - 4.0 * textboxSize.x - textSize.x) / dragCount - 17.5);
        if (ImGui::CollapsingHeader("Material Component")) {
            // Base color
            {
                DrawCustomTextShape("Color", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("R", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, baseColor.r, 0.001, 0.0, 1.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("G", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, baseColor.g, 0.001, 0.0, 1.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("B", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, baseColor.b, 0.001, 0.0, 1.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("A", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, baseColor.a, 0.001, 0.0, 1.0, "%.2f");
            }
            ImGui::ColorEdit3("Emissive", &emissive[0]);
            DrawCustomSizedDragger(191.5, metallic, 0.01, 0.001, 1.0);
            DrawCustomSizedDragger(191.5, roughness, 0.01, 0.001, 1.0);
        }
    }

    void HierarchyPanel::DrawComponent(MeshComponent *comp) {
        if (ImGui::CollapsingHeader("Mesh Component")) {
        }
    }

    void HierarchyPanel::DrawComponent(LightComponent *comp) {
        auto& light = comp->m_Light;
        auto diffuse = light.GetDiffuse();
        auto specular = light.GetSpecular();
        auto constant = light.GetConstant();
        auto linear = light.GetLinear();
        auto quadratic = light.GetQuadratic();

        if (ImGui::CollapsingHeader("Light Component")) {
            if (ImGui::ColorEdit3("Diffuse", &diffuse[0])) {
                light.SetDiffuse(diffuse);
            }
            if (ImGui::ColorEdit3("Specular", &specular[0])) {
                light.SetSpecular(specular);
            }
            // Diffuse
            // if (ImGui::DragFloat4("Diffuse", &diffuse[0], 0.01, 0.001, 1.0)) {
                // light.SetDiffuse(diffuse);
            // }
            // Specular
            // if (ImGui::DragFloat4("Specular", &specular[0], 0.01, 0.001, 1.0)) {
                // light.SetSpecular(specular);
            // }

            // Attenuation Parameters
            if (light.GetType() == LightType::POINT) {
                ImGui::BeginGroup();
                ImGui::TextColored(ImVec4(1.0, 1.0, 0.2, 1.0), "Attenuation Parameters");
                if (ImGui::DragFloat("Constant", &constant, 0.001, 0.00001, 1.0)) {
                    light.SetConstant(constant);
                }

                if (ImGui::DragFloat("Linear", &linear, 0.001, 0.00001, 1.0)) {
                    light.SetLinear(linear);
                }

                if (ImGui::DragFloat("Quadratic", &quadratic, 0.1, 0.00001, 1000.0)) {
                    light.SetQuadratic(quadratic);
                }
                ImGui::EndGroup();
            }

            // Light type selection window
            const char* arr[3] = { "Point", "Directional", "Spot" };
            if (ImGui::BeginCombo("##LightTypes",  arr[static_cast<int>(light.GetType())])) {
                // TODO: Add SetType function and change the current light with a new one
                if (ImGui::Selectable("Point")) {
                    // light.SetType(LightType::POINT);
                }
                if (ImGui::Selectable("Directional")) {
                    // light.SetType(LightType::DIRECTIONAL);
                }
                if (ImGui::Selectable("Spot")) {
                    // light.SetType(LightType::SPOT);
                }
                ImGui::EndCombo();
            }
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

    void HierarchyPanel::DrawCustomTextShape(const std::string &text, ImVec2 boxSize, ImVec4 color, bool textColorActive, ImVec4 textColor) {
        const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, color);
        ImGui::BeginChild(ConcatStr("##readonly", std::to_string(m_IDcounter++)).c_str(), boxSize);
        // Center the text
        ImGui::SetCursorPos(ImVec2((boxSize.x - textSize.x) * 0.5f, (boxSize.y - textSize.y) * 0.5f));
        textColorActive ? ImGui::TextColored(textColor, text.c_str()) : ImGui::Text(text.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor(1);
    }

    void HierarchyPanel::DrawCustomSizedDragger(float dragWidth, float& val, float speed, float v_min, float v_max, const char* format) {
        ImGui::SetNextItemWidth(dragWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 7));
        ImGui::DragFloat(ConcatStr("##dragger", std::to_string(m_IDcounter++)).c_str(), &val, speed, v_min, v_max, format);
        ImGui::PopStyleVar();
    }

}
