//
// Created by pointerlost on 10/17/25.
//
#include "Editor.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "EditorState.h"
#include "InspectorPanel.h"
#include "HierarchyPanel.h"
#include "Assets/AssetManager.h"
#include "Core/CMakeConfig.h"
#include "Assets/FileManager.h"
#include "Core/IPhysicsBackend.h"
#include "Core/Services.h"
#include "Platform/GLFW/GLFWwindow.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "Math/Math.h"
#include "Scene/Entity.h"
#include "scenes/SandboxScene.h"


namespace Real::UI {

    Editor::Editor()
    {
        m_Panels.push_back(CreateScope<InspectorPanel>());
        m_Panels.push_back(CreateScope<HierarchyPanel>());
    }

    void Editor::Init(core::IWindow* window) {
        // TODO: need an ImGui backend interface, because we need specific library like GLFW??????
        m_Window = dynamic_cast<platform::glfw::GLFWWindow*>(window);
        if (!m_Window) {
            throw std::runtime_error("Editor requires GLFWWindow");
        }

        // Setup context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_Window->GetNativeHandle()), true);
        ImGui_ImplOpenGL3_Init();

        // Init custom font style
        InitDarkTheme();

        Real::Editor::SandboxScene::Load(*m_Scene);
    }

    void Editor::BeginFrame(float dt) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        // Scene window props
        // Hierarchy,Inspector width = SCREEN_WIDTH / 5 + 31.0
        // MenuBarPanel height = 25.0
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 5 + 31.0, 25.0));
        ImGui::SetNextWindowSize(ImVec2(m_SceneWindowSize.x, m_SceneWindowSize.y));

        constexpr auto windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoCollapse   | ImGuiWindowFlags_NoTitleBar;
        // Draw main scene window into Editor Window
        ImGui::Begin("Scene", nullptr, windowFlags);
    }

    void Editor::Update(float dt) {
        UpdateInputUI();
        UpdateGizmoLogic();
    }

    void Editor::RenderUI() {
        // Draw UI
        Render();

        ImGui::End();

        for (const auto& panel : m_Panels) {
            panel->OnImGuiRender();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Editor::EndFrame() {
    }

    void Editor::OnAttachScene(Scene *scene) {
        m_Scene = scene;

        for (const auto& panel : m_Panels) {
            panel->OnSceneAttached(m_Scene);
        }
    }

    void Editor::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Editor::RenderMenuBar() {
        ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 5);
        ImGui::BeginMainMenuBar();

        RenderFileBar();
        RenderEditBar();
        RenderViewBar();
        RenderDebugBar();
        RenderHelpBar();

        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();
    }

    void Editor::RenderFileBar() {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
            }

            if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
            }

            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
            }

            if (ImGui::MenuItem("Save Scene As...")) {
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) {
                Services::GetEditorState()->Running = false;
            }

            ImGui::EndMenu();
        }
    }

    void Editor::RenderEditBar() {
        if (ImGui::BeginMenu("Edit")) {

            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Y");

            ImGui::Separator();

            ImGui::MenuItem("Duplicate", "Ctrl+D");
            ImGui::MenuItem("Delete", "Del");

            ImGui::Separator();

            ImGui::MenuItem("Editor Settings");

            ImGui::EndMenu();
        }
    }

    void Editor::RenderViewBar() {
        if (ImGui::BeginMenu("View")) {

            ImGui::MenuItem("Show Grid",      nullptr, &Services::GetEditorState()->debug.showGrid);
            ImGui::MenuItem("Show Colliders", nullptr, &Services::GetEditorState()->debug.showColliders);
            ImGui::MenuItem("Show Physics",   nullptr, &Services::GetEditorState()->debug.showPhysics);

            ImGui::Separator();

            ImGui::MenuItem("Perspective");
            ImGui::MenuItem("Orthographic");

            ImGui::EndMenu();
        }
    }

    void Editor::RenderDebugBar() {
        if (ImGui::BeginMenu("Debug")) {

            ImGui::MenuItem("Show Colliders", nullptr, &Services::GetEditorState()->debug.showColliders);
            ImGui::MenuItem("Show Physics Shapes", nullptr, &Services::GetEditorState()->debug.showPhysics);
            ImGui::MenuItem("Show Bounds", nullptr, &Services::GetEditorState()->debug.showBounds);

            ImGui::Separator();

            ImGui::MenuItem("Show Camera Frustums");
            ImGui::MenuItem("Show Light Volumes");

            ImGui::Separator();

            ImGui::MenuItem("Enable Debug Renderer");

            ImGui::EndMenu();
        }
    }

    void Editor::RenderHelpBar() {
        if (ImGui::BeginMenu("Help")) {

            if (ImGui::MenuItem("About")) {
            }

            if (ImGui::MenuItem("Controls")) {
            }

            ImGui::Separator();

            ImGui::MenuItem("Documentation");

            ImGui::EndMenu();
        }
    }

    void Editor::DrawPerformanceProfile() {
        if (Input::IsKeyPressed(REAL_KEY_F11)) m_OpenPerformanceProfile = !m_OpenPerformanceProfile;
        if (m_OpenPerformanceProfile) return;
        // const auto fps = "FPS: " + std::to_string(Services::GetEditorTimer()->GetFPS());
        // ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), fps.c_str());
    }

    void Editor::UpdateInputUI() {
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            Services::GetEditorState()->selectedEntity = nullptr;
        }

        if (Input::IsKeyPressed(REAL_KEY_F3)) {
            Services::GetEditorState()->FpsMode = !Services::GetEditorState()->FpsMode;
        }
    }

    void Editor::InitDarkTheme() {
        // TODO: Background of text colors can be change
        ImGui::GetStyle().Colors[ImGuiCol_Header]         = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]   = ImVec4(0.1568, 0.6294, 0.1137, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]  = ImVec4(0.1765, 0.2157, 0.2823, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]  = ImVec4(0.19, 0.07, 0.79, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg]        = ImVec4(0.09, 0.07, 0.09, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_Text]           = ImVec4(5.0, 1.0, 1.0, 0.87);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBg]        = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2620, 0.3250, 0.28260, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.3712, 0.4035, 0.3907, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg]       = ImVec4(0.03954, 0.03914, 0.03934, 1.0);
    }

    void Editor::RenderSceneGizmos() {
        const auto* editorState = Services::GetEditorState();
        if (!editorState->selectedEntity)
            return;

        const ImVec2 min = ImGui::GetWindowContentRegionMin();
        const ImVec2 max = ImGui::GetWindowContentRegionMax();
        const ImVec2 pos = ImGui::GetWindowPos();

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        // screen-space rectangle of the Scene window
        ImGuizmo::SetRect(
            pos.x + min.x,
            pos.y + min.y,
            max.x - min.x,
            max.y - min.y
        );

        const Entity entity = *editorState->selectedEntity;
        auto& tc            = entity.GetComponentUnchecked<TransformComponent>();
        auto& camera        = editorState->editorCamera->GetComponent<CameraComponent>().camera;

        const bool hasCollider = entity.HasComponent<ColliderComponent>();
        const bool hasPhysics  = entity.HasComponent<RigidbodyComponent>();

        // Decide whether ENTITY transform is editable
        bool canEditEntityTransform = true;

        if (hasPhysics) {
            const auto& pb = entity.GetComponentUnchecked<RigidbodyComponent>();
            if (pb.desc.type == core::BodyDesc::Type::Dynamic) {
                canEditEntityTransform = false;
            }
        }

        /*
         * PhysX rule:
         * A shape never exists in world space
         * A shape is always relative to its actor
         * For example: shapeLocalPose = PxTransform(localPosition, localRotation);
         *
         * Static collider  -> (Real)  own transform
         * Dynamic collider -> (PhysX) own transform
         * Colliders always lives in actor space, Gizmos always edits world space
        */

        // Build actor (entity) world matrix without scaling,
        // This is intentional, because PhysX does not allow scaling actors
        const math::Mat4 actorWorld =
            math::Mat4::Translate(tc.transform.GetWorldPosition()) * tc.transform.GetWorldRotation().ToMat4();

        // Choose gizmo matrix
        math::Mat4 gizmoMatrix = tc.transform.GetWorldMatrix(); // Use entity transform as default

        // If entity has a ColliderComponent, let Physx manages transform of entity's collider transform
        if (!m_EditEntityTransform && hasCollider) {
            const auto& [desc, handle]       = entity.GetComponentUnchecked<ColliderComponent>();
            const auto& [position, rotation] = desc.localTransform;

            // ImGuizmo only understands world matrices
            gizmoMatrix = actorWorld * math::Mat4::Translate(position) * rotation.ToMat4();
        }

        // Draw gizmo
        ImGuizmo::Manipulate(camera.GetView().ValuePtr(), camera.GetProjection().ValuePtr(),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::WORLD, gizmoMatrix.ValuePtr()
        );

        if (!ImGuizmo::IsUsing())
            return;

        // Decompose result
        math::Vec3 translation{}, scale{};
        math::Quat rotation{};
        math::DecomposeTransform(gizmoMatrix, translation, rotation, scale);

        // ENTITY TRANSFORM EDIT
        if (m_EditEntityTransform || !hasCollider) {
            if (!canEditEntityTransform) {
                return; // Dynamic body not editable for transform, Because PhysX already handles this.
            }

            tc.transform.SetLocalPosition(translation);
            tc.transform.SetLocalRotation(rotation);
            tc.transform.SetLocalScale(scale);
            return;
        }

        // COLLIDER LOCAL TRANSFORM EDIT
        auto& cc = entity.GetComponentUnchecked<ColliderComponent>();
        if (cc.desc.shape == core::ShapeDesc::Shape::Box) {
        }

        const math::Mat4 invActorWorld = actorWorld.Inverted();
        math::Mat4 localMatrix         = invActorWorld * gizmoMatrix;

        math::Vec3 localPos  {};
        math::Vec3 localScale{};
        math::Quat localRot  {};
        math::DecomposeTransform(localMatrix, localPos, localRot, localScale);

        cc.desc.localTransform = { localPos, localRot };
        // PhysX works in half extents, Debug mesh works in full extents,
        // in this case scale with 0.5 before sending to PhysX
        cc.desc.size = localScale * 0.5f; // full -> half extents
    }

    void Editor::UpdateGizmoLogic() {
        // Toggle gizmo target (entity <-> collider)
        if (Input::IsKeyPressed(REAL_KEY_Y))
            m_EditEntityTransform = !m_EditEntityTransform;

        // Select gizmo operation
        if (Input::IsKeyPressed(REAL_KEY_E))
            m_GizmoType = ImGuizmo::TRANSLATE;
        else if (Input::IsKeyPressed(REAL_KEY_R))
            m_GizmoType = ImGuizmo::ROTATE;
        else if (Input::IsKeyPressed(REAL_KEY_T))
            m_GizmoType = ImGuizmo::SCALE;
    }

    void Editor::DebugGizmos() {
        if (!Services::GetEditorState()->selectedEntity) return;

        if (ImGuizmo::IsUsing()) {
            ImGui::Text("Using gizmo");
        } else {
            ImGui::Text(ImGuizmo::IsOver() ? "Over Gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over TRANSLATE Gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over ROTATE Gizmo" : "");
            ImGui::SameLine();
            ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over SCALE Gizmo" : "");
        }
    }

    void Editor::Render() {
        RenderMenuBar();
        RenderSceneGizmos();
        DrawPerformanceProfile();
        // DebugGizmos();
    }

}
