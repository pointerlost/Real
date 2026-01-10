//
// Created by pointerlost on 10/17/25.
//
#include "Editor/HierarchyPanel.h"
#include <imgui.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <imgui_internal.h>
#include <glm/gtx/string_cast.hpp>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Editor/EditorPanel.h"
#include "Editor/EditorState.h"
#include "Graphics/Material.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

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

        ImGui::PushFont(Services::GetAssetManager()->GetFontStyle("Ubuntu-Bold"));
        if (entity->HasComponent<TagComponent>()) {
            DrawComponent(&entity->GetComponentUnchecked<TagComponent>(), scene);
        }
        if (entity->HasComponent<TransformComponent>()) {
            DrawComponent(&entity->GetComponentUnchecked<TransformComponent>(), scene);
        }
        if (entity->HasComponent<MeshRendererComponent>()) {
            DrawComponent(&entity->GetComponent<MeshRendererComponent>(), scene);
        }
        if (entity->HasComponent<LightComponent>()) {
            DrawComponent(&entity->GetComponent<LightComponent>(), &entity->GetComponent<TransformComponent>(), scene);
        }
        if (entity->HasComponent<CameraComponent>()) {
            DrawComponent(&entity->GetComponent<CameraComponent>(), scene);
        }
        ImGui::PopFont();

        // Reset
        m_IDcounter = 0;
    }

    void HierarchyPanel::DrawComponent(TagComponent *comp, Scene* scene) {
        if (ImGui::CollapsingHeader("Tag Component")) {
            // Max 21 character
            ImGui::InputText("Tag" ,comp->m_Tag.data(), 21);
        }
    }

    void HierarchyPanel::DrawComponent(TransformComponent *comp, Scene* scene) {
        auto& transform = comp->m_Transform;
        auto position = transform.GetTranslate();
        auto rotate   = transform.GetRotationEuler();
        auto scale    = transform.GetScale();

        constexpr auto textboxSize = ImVec2(25.0, 30.0);
        constexpr auto textSize    = ImVec2(70.0, 30.0);
        constexpr float dragCount = 3.0;
        const auto dragSize = static_cast<float>((m_SizeX - 3.0 * textboxSize.x - textSize.x) / dragCount - 20.0);
        if (ImGui::CollapsingHeader("Transform Component")) {
            // Translate
            {
                DrawCustomTextShape("Position", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.x, 0.1, -360.0, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.y, 0.1, -360.0, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, position.z, 0.1, -360.0, 360.0, "%.2f");

                transform.SetTranslate(position);
            }

            // Rotate
            {
                DrawCustomTextShape("Rotation", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.x, 0.1, -360.0, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.y, 0.1, -360.0, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, rotate.z, 0.1, -360.0, 360.0, "%.2f");

                transform.SetRotationEuler(rotate);
            }

            // Scale
            {
                DrawCustomTextShape("Scale", textSize, ImVec4(0.1019, 0.1568, 0.1372, 1.0));
                ImGui::SameLine();
                DrawCustomTextShape("X", textboxSize, ImVec4(1.0, 0.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.x, 0.1, 0.01, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Y", textboxSize, ImVec4(0.0, 1.0, 0.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.y, 0.1, 0.01, 360.0, "%.2f");
                ImGui::SameLine();
                DrawCustomTextShape("Z", textboxSize, ImVec4(0.0, 0.0, 1.0, 1.0), true, ImVec4(0.05, 0.05, 0.05, 1.0));
                ImGui::SameLine();
                DrawCustomSizedDragger(dragSize, scale.z, 0.1, 0.01, 360.0, "%.2f");

                transform.SetScale(scale);
            }
        }
    }
    //
    // void HierarchyPanel::DrawComponent(MaterialComponent *comp, Scene* scene) {
    //
    // }

    void HierarchyPanel::DrawComponent(const MeshRendererComponent *comp, Scene* scene) {
        const auto& am = Services::GetAssetManager();

        if (!ImGui::CollapsingHeader("Mesh Component", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        const auto mat = am->GetMaterialInstance(comp->m_MaterialInstanceUUIDs[0]);
        if (!mat)
            return;

        auto& baseColor = mat->m_BaseColorFactor;
        auto& factors = mat->m_ORMFactor; // x=AO, y=Roughness, z=Metallic, w=padding

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));

        if (ImGui::BeginTable("MaterialFactors", 5,
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_NoHostExtendX))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("R", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("G", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("B", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("A", ImGuiTableColumnFlags_WidthFixed, 70.0f);

            // ================= Header =================
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.9f, 1.0f), "Property");

            constexpr ImVec4 headerColors[] = {
                ImVec4(0.9f, 0.4f, 0.4f, 1.0f), // R
                ImVec4(0.4f, 0.9f, 0.4f, 1.0f), // G
                ImVec4(0.4f, 0.5f, 0.9f, 1.0f), // B
                ImVec4(0.7f, 0.7f, 0.7f, 1.0f)  // A
            };

            for (int i = 1; i < 5; i++) {
                ImGui::TableSetColumnIndex(i);
                ImGui::PushStyleColor(ImGuiCol_Text, headerColors[i-1]);
                ImGui::Text("%s", i == 1 ? "R" : i == 2 ? "G" : i == 3 ? "B" : "A");
                ImGui::PopStyleColor();
            }

            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 40, 40, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(60, 60, 60, 200));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(80, 80, 80, 255));

            // Base Color
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Base Color");

            // Show color preview
            ImGui::SameLine();
            ImGui::ColorButton("##BaseColorPreview",
                ImVec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a),
                ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip,
                ImVec2(16, 16)
            );

            constexpr float dragSpeed = 0.005f;
            const auto format = "%.3f";

            // Red channel
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(100, 40, 40, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(120, 50, 50, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##BaseR", &baseColor.r, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Green channel
            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 100, 40, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 120, 50, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##BaseG", &baseColor.g, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Blue channel
            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 40, 100, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 50, 120, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##BaseB", &baseColor.b, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Alpha channel
            ImGui::TableSetColumnIndex(4);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(80, 80, 80, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(100, 100, 100, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat("##BaseA", &baseColor.a, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Surface Factors
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Surface");

            // AO
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat("##AO", &factors.x, 0.005f, 0.0f, 1.0f, "AO: %.3f")) {
            }

            // Roughness
            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat("##Roughness", &factors.y, 0.005f, 0.0f, 1.0f, "R: %.3f")) {
            }

            // Metallic
            ImGui::TableSetColumnIndex(3);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat("##Metallic", &factors.z, 0.005f, 0.0f, 1.0f, "M: %.3f")) {
            }

            // Empty column for alignment
            ImGui::TableSetColumnIndex(4);
            ImGui::TextDisabled("-");

            ImGui::PopStyleColor(3); // FrameBg colors
            ImGui::EndTable();
        }

        ImGui::PopStyleVar(2);

        // Optional: Add a color picker for the base color
        if (ImGui::Button("Color Picker...", ImVec2(-FLT_MIN, 0))) {
            ImGui::OpenPopup("BaseColorPicker");
        }

        if (ImGui::BeginPopup("BaseColorPicker")) {
            ImGui::ColorPicker4("Base Color", &baseColor.r,
                ImGuiColorEditFlags_DisplayRGB |
                ImGuiColorEditFlags_DisplayHSV |
                ImGuiColorEditFlags_AlphaBar |
                ImGuiColorEditFlags_PickerHueBar);
            ImGui::EndPopup();
        }
    }

    void HierarchyPanel::DrawComponent(LightComponent *comp, TransformComponent* transform, Scene* scene) {
        auto& light = comp->m_Light;
        auto radiance    = light.GetRadiance();
        auto constant    = light.GetConstant();
        auto linear      = light.GetLinear();
        auto quadratic   = light.GetQuadratic();
        auto cutOff      = light.GetCutOff();
        auto outerCutOff = light.GetOuterCutOff();

        if (ImGui::CollapsingHeader("Light Component")) {
            const auto lightType = light.GetType();

            if (ImGui::ColorEdit3("Radiance", &radiance[0])) {
                light.SetRadiance(radiance);
            }

            // Attenuation Parameters
            if (lightType == LightType::POINT) {
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

            // Spot light parameters
            if (lightType == LightType::SPOT) {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode("Spot Properties")) {
                    DrawCustomTextShape("Inner Angle (CutOff)", ImVec2(200,30), ImVec4(0.03954, 0.03914, 0.03934, 1.0), false, ImVec4(0.75, 0.75, 0.2, 1.0));
                    ImGui::SameLine();
                    DrawCustomSizedDragger(75.0, cutOff, 0.05, 1.0, outerCutOff, "%.2f");
                    light.SetCutOff(cutOff);

                    DrawCustomTextShape("Outer Angle (OuterCutOff)", ImVec2(200,30), ImVec4(0.03954, 0.03914, 0.03934, 1.0), false, ImVec4(0.75, 0.75, 0.2, 1.0));
                    ImGui::SameLine();
                    DrawCustomSizedDragger(75.0, outerCutOff, 0.05, cutOff, 90.0, "%.2f");
                    light.SetOuterCutOff(outerCutOff);

                    ImGui::TreePop();
                }
            }

            // Light type selection window
            const char* arr[3] = { "Point", "Directional", "Spot" };
            if (ImGui::BeginCombo("##LightTypes",  arr[static_cast<int>(light.GetType())])) {
                // TODO: Store the old light properties to be changed with a new one
                // In this case we are creating a new one
                if (ImGui::Selectable("Point")) {
                    comp->m_Light = Light{LightType::POINT};
                }
                if (ImGui::Selectable("Directional")) {
                    comp->m_Light = Light{LightType::DIRECTIONAL};
                }
                if (ImGui::Selectable("Spot")) {
                    comp->m_Light = Light{LightType::SPOT};
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::Selectable("Show Light Direction")) {
            if (light.GetType() == LightType::DIRECTIONAL) {
            }
        }
    }

    void HierarchyPanel::DrawComponent(CameraComponent *comp, Scene* scene) {
        auto& camera = comp->m_Camera;
        auto near   = camera.GetNear();
        auto far    = camera.GetFar();
        auto fov    = camera.GetFOV();
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

        if (ImGui::CollapsingHeader("Editor Camera State")) {
            const auto& view = scene->GetAllEntitiesWith<IDComponent, TagComponent, CameraComponent>();

            ImGui::BeginListBox("##Cameras");
            for (auto [entity, id, tagComp, cc] : view.each()) {
                auto tag = tagComp.m_Tag;
                if (scene->GetEntityWithUUID(id.m_UUID) == Services::GetEditorState()->camera) {
                    tag += " (Current)";
                }
                if (ImGui::Selectable(tag.c_str())) {
                    Services::GetEditorState()->camera = scene->GetEntityWithUUID(id.m_UUID);
                    break;
                }
            }
            ImGui::EndListBox();
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
