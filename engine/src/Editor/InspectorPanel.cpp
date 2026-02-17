//
// Created by pointerlost on 10/17/25.
//
#include "Editor/InspectorPanel.h"
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
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real::UI {

    InspectorPanel::InspectorPanel()
    {
    }

    void InspectorPanel::BeginFrame() {
    }

    void InspectorPanel::Render(Scene* scene, opengl::OpenGLRenderer* renderer) {
        ImGui::SetNextWindowSize(ImVec2(m_SizeX, m_SizeY));
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - m_SizeX, 25), ImGuiCond_Always);
        ImGui::Begin("Hierarchy", &m_Open, ImGuiWindowFlags_NoResize);

        DrawComponents(scene);

        ImGui::End();
    }

    void InspectorPanel::Shutdown() {
    }

    void InspectorPanel::DrawComponents(Scene* scene) {
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
        if (entity->HasComponent<ColliderComponent>()) {
            DrawComponent(*entity, &entity->GetComponent<ColliderComponent>(), scene);
        }
        if (entity->HasComponent<RigidbodyComponent>()) {
            DrawComponent(*entity, &entity->GetComponent<RigidbodyComponent>(), scene);
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

    void InspectorPanel::DrawComponent(TagComponent *comp, Scene* scene) {
        if (ImGui::CollapsingHeader("Tag Component")) {
            // Max 21 character
            ImGui::InputText("Tag" ,comp->m_Tag.data(), 21);
        }
    }

    void InspectorPanel::DrawComponent(TransformComponent *comp, Scene* scene) {
        auto& transform = comp->transform;
        auto position = transform.position;
        auto rotate   = transform.rotation;
        auto scale    = transform.scale;

        constexpr auto textboxSize = ImVec2(25.0, 30.0);
        constexpr auto textSize    = ImVec2(70.0, 30.0);
        constexpr f32 dragCount = 3.0;
        const auto dragSize = static_cast<f32>((m_SizeX - 3.0 * textboxSize.x - textSize.x) / dragCount - 20.0);
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

                transform.SetPosition(position);
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

                transform.SetRotation(rotate);
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

    void InspectorPanel::DrawComponent(const MeshRendererComponent *comp, Scene* scene) {
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
                // RGBA
                ImVec4(baseColor.x, baseColor.y, baseColor.z, baseColor.w),
                ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip,
                ImVec2(16, 16)
            );

            constexpr f32 dragSpeed = 0.005f;
            const auto format = "%.3f";

            // Red channel
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(100, 40, 40, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(120, 50, 50, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##BaseR", &baseColor.x, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Green channel
            ImGui::TableSetColumnIndex(2);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 100, 40, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 120, 50, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##BaseG", &baseColor.y, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Blue channel
            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 40, 100, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 50, 120, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##BaseB", &baseColor.z, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Alpha channel
            ImGui::TableSetColumnIndex(4);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(80, 80, 80, 128));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(100, 100, 100, 200));
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::DragFloat3("##BaseA", &baseColor.w, dragSpeed, 0.0f, 1.0f, format);
            ImGui::PopStyleColor(2);

            // Surface Factors
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Surface");

            // AO
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##AO", &factors.x, 0.005f, 0.0f, 1.0f, "AO: %.3f")) {
            }

            // Roughness
            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##Roughness", &factors.y, 0.005f, 0.0f, 1.0f, "R: %.3f")) {
            }

            // Metallic
            ImGui::TableSetColumnIndex(3);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::DragFloat3("##Metallic", &factors.z, 0.005f, 0.0f, 1.0f, "M: %.3f")) {
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
            ImGui::ColorPicker4("Base Color", &baseColor.x,
                ImGuiColorEditFlags_DisplayRGB |
                ImGuiColorEditFlags_DisplayHSV |
                ImGuiColorEditFlags_AlphaBar |
                ImGuiColorEditFlags_PickerHueBar);
            ImGui::EndPopup();
        }
    }

    void InspectorPanel::DrawComponent(Entity& entity, ColliderComponent *comp, Scene *scene) {
        if (!ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        bool dirty   = false;
        bool rebuild = false;

        // Enabled toggle (attach / detach)
        if (ImGui::Checkbox("Enabled", &comp->enabled)) {
            rebuild = true;
        }

        ImGui::Separator();

        // Collider shape type
        const char* shapeNames[] = { "Box", "Sphere", "Capsule" };
        int currentShape = static_cast<int>(comp->shape);

        if (ImGui::Combo("Shape", &currentShape, shapeNames, IM_ARRAYSIZE(shapeNames))) {
            comp->shape = static_cast<physics::ColliderShape>(currentShape);
            rebuild = true;
        }

        // Geometry
        if (comp->shape == physics::ColliderShape::Box) {
            ImGui::Text("Size");
            if (ImGui::DragFloat3("##Size", &comp->size.x, 0.01f, 0.001f)) {
                dirty = true;
            }
        }

        ImGui::Separator();

        // Local offset
        ImGui::Text("Center");
        if (ImGui::DragFloat3("##Center", &comp->localPosition.x, 0.01f)) {
            dirty = true;
        }

        ImGui::Text("Rotation");
        math::Vec3 euler = math::ToEulerDegrees(comp->localRotation);
        if (ImGui::DragFloat3("##Rotation", &euler.x, 0.5f)) {
            comp->localRotation = math::Quat::FromEulerDegrees(euler);
            dirty = true;
        }

        ImGui::Separator();

        // Trigger
        if (ImGui::Checkbox("Is Trigger", &comp->isTrigger)) {
            rebuild = true; // PhysX flags need rebuild
        }

        ImGui::Separator();

        // Runtime debug info (read-only)
        // ImGui::TextDisabled("Actor:  %p", comp->actor);
        // ImGui::TextDisabled("Shape:  %p", comp->shapeHandle);
        // ImGui::TextDisabled("Attached: %s", comp->attached ? "Yes" : "No");
        //
        // ImGui::SeparatorText("Debug");
        // ImGui::Checkbox("Show Collider", &comp->debug.show);
        // ImGui::Checkbox("Show Collider Bounds", &comp->debug.showBounds);

        if (!rebuild && !dirty) return;

        scene->GetEvents().OnColliderChanged.Emit(
            entity,
            rebuild ? physics::ColliderChangeType::Rebuild : physics::ColliderChangeType::Dirty
        );
    }

    void InspectorPanel::DrawComponent(Entity &entity, RigidbodyComponent *comp, Scene *scene) {
        if (entity.HasComponent<RigidbodyComponent>()) {
            if (ImGui::CollapsingHeader("Physics Body", ImGuiTreeNodeFlags_DefaultOpen)) {

                const char* bodyTypes[] = { "Static", "Dynamic", "Kinematic" };
                int current = static_cast<int>(comp->type);

                if (ImGui::Combo("Body Type", &current, bodyTypes, IM_ARRAYSIZE(bodyTypes))) {
                    comp->type = static_cast<physics::BodyType>(current);

                    // IMPORTANT: body type change requires actor recreation
                    scene->GetEvents().OnPhysicsBodyChanged.Emit(entity, *comp);
                }

                if (comp->type == physics::BodyType::Dynamic) {
                    if (ImGui::DragFloat3("Mass", &comp->mass, 0.1f, 0.01f)) {
                        scene->GetEvents().OnPhysicsBodyChanged.Emit(entity, *comp);
                    }
                }
            }
        }
    }

    void InspectorPanel::DrawComponent(LightComponent *comp, TransformComponent* transform, Scene* scene) {
        auto& light = comp->m_Light;
        auto radiance    = light.GetRadiance();
        auto constant    = light.GetConstant();
        auto linear      = light.GetLinear();
        auto quadratic   = light.GetQuadratic();
        auto cutOff      = light.GetCutOff();
        auto outerCutOff = light.GetOuterCutOff();

        if (ImGui::CollapsingHeader("Light Component")) {
            const auto lightType = light.GetType();

            if (ImGui::ColorEdit3("Radiance", radiance.ValuePtr())) {
                light.SetRadiance(radiance);
            }

            // Attenuation Parameters
            if (lightType == Light::Mode::POINT) {
                ImGui::BeginGroup();
                ImGui::TextColored(ImVec4(1.0, 1.0, 0.2, 1.0), "Attenuation Parameters");
                if (ImGui::DragFloat3("Constant", &constant, 0.001, 0.00001, 1.0)) {
                    light.SetConstant(constant);
                }

                if (ImGui::DragFloat3("Linear", &linear, 0.001, 0.00001, 1.0)) {
                    light.SetLinear(linear);
                }

                if (ImGui::DragFloat3("Quadratic", &quadratic, 0.1, 0.00001, 1000.0)) {
                    light.SetQuadratic(quadratic);
                }
                ImGui::EndGroup();
            }

            // Spot light parameters
            if (lightType == Light::Mode::SPOT) {
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
                    comp->m_Light = Light{Light::Mode::POINT};
                }
                if (ImGui::Selectable("Directional")) {
                    comp->m_Light = Light{Light::Mode::DIRECTIONAL};
                }
                if (ImGui::Selectable("Spot")) {
                    comp->m_Light = Light{Light::Mode::SPOT};
                }
                ImGui::EndCombo();
            }
        }

        if (ImGui::Selectable("Show Light Direction")) {
            if (light.GetType() == Light::Mode::DIRECTIONAL) {
            }
        }
    }

    void InspectorPanel::DrawComponent(CameraComponent *comp, Scene* scene) {
        auto& camera = comp->m_Camera;
        auto near   = camera.GetNear();
        auto far    = camera.GetFar();
        auto fov    = camera.GetFOV();
        auto aspect = camera.GetAspect();

        if (ImGui::CollapsingHeader("Camera Component")) {
            // TODO: redesign drag f32s with new values
            if (ImGui::DragFloat3("Near Plane", &near, 0.01, 0.001, 500.0)) {
                camera.SetNear(near);
            }
            if (ImGui::DragFloat3("Far Plane", &far, 0.05, 0.001, 100000.0)) {
                camera.SetFar(far);
            }
            if (ImGui::DragFloat3("FOV", &fov, 0.01, 0.001, 500.0)) { // Change the values
                camera.SetFOV(fov);
            }
            if (ImGui::DragFloat3("Aspect", &aspect, 0.01, 0.001, 500.0)) { // Change the values
                camera.SetFar(aspect);
            }
        }

        if (ImGui::CollapsingHeader("Editor Camera State")) {
            const auto& view = scene->GetAllEntitiesWith<IDComponent, TagComponent, CameraComponent>();

            ImGui::BeginListBox("##Cameras");
            for (auto [entity, id, tagComp, cc] : view.each()) {
                auto tag = tagComp.m_Tag;
                if (scene->GetEntityWithUUID(id.m_UUID) == Services::GetEditorState()->editorCamera) {
                    tag += " (Current)";
                }
                if (ImGui::Selectable(tag.c_str())) {
                    Services::GetEditorState()->editorCamera = scene->GetEntityWithUUID(id.m_UUID);
                    break;
                }
            }
            ImGui::EndListBox();
        }
    }

    void InspectorPanel::DrawCustomTextShape(const String &text, ImVec2 boxSize, ImVec4 color, bool textColorActive, ImVec4 textColor) {
        const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        ImGui::PushStyleColor(ImGuiCol_ChildBg, color);
        ImGui::BeginChild(ConcatStr("##readonly", std::to_string(m_IDcounter++)).c_str(), boxSize);
        // Center the text
        ImGui::SetCursorPos(ImVec2((boxSize.x - textSize.x) * 0.5f, (boxSize.y - textSize.y) * 0.5f));
        textColorActive ? ImGui::TextColored(textColor, text.c_str()) : ImGui::Text(text.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor(1);
    }

    void InspectorPanel::DrawCustomSizedDragger(f32 dragWidth, f32& val, f32 speed, f32 v_min, f32 v_max, const char* format) {
        ImGui::SetNextItemWidth(dragWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 7));
        ImGui::DragFloat3(ConcatStr("##dragger", std::to_string(m_IDcounter++)).c_str(), &val, speed, v_min, v_max, format);
        ImGui::PopStyleVar();
    }

}
