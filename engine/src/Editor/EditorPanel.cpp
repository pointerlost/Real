//
// Created by pointerlost on 10/17/25.
//
#include "Editor/EditorPanel.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "PxShape.h"
#include "Core/AssetManager.h"
#include "Core/CMakeConfig.h"
#include "Core/file_manager.h"
#include "Core/Services.h"
#include "Core/Timer.h"
#include "Editor/EditorState.h"
#include "Editor/InspectorPanel.h"
#include "Editor/HierarchyPanel.h"
#include "geometry/PxBoxGeometry.h"
#include "Graphics/Renderer.h"
#include "Graphics/Debug/DebugRenderer.h"
#include "Input/Keycodes.h"
#include "ImGuizmo/ImSequencer.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "ImGuizmo/GraphEditor.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Util/Util.h"

namespace Real::UI {

    EditorPanel::EditorPanel(Graphics::Window *window, InspectorPanel* hierarchyPanel, HierarchyPanel* inspectorPanel)
        : m_Window(window), m_HierarchyPanel(hierarchyPanel), m_InspectorPanel(inspectorPanel)
    {
        // Setup context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGLFWWindow(), true);
        ImGui_ImplOpenGL3_Init();

        // Init custom font style
        InitDarkTheme();
        InitFontStyle();
    }

    void EditorPanel::BeginFrame() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void EditorPanel::Update() {
        UpdateInputUI();
        UpdateGizmoLogic();
    }

    void EditorPanel::Render(Scene* scene, opengl::OpenGLRenderer* renderer) {
        // Scene window props
        // Hierarchy,Inspector width = SCREEN_WIDTH / 5 + 31.0
        // MenuBarPanel height = 25.0
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 5 + 31.0, 25.0));
        ImGui::SetNextWindowSize(m_SceneWindowSize);

        constexpr auto windowFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoCollapse   | ImGuiWindowFlags_NoTitleBar;
        // Draw main scene window into Editor Window
        ImGui::Begin("Scene", nullptr, windowFlags);
        // Main Scene window
        renderer->Render(Services::GetEditorState()->editorCamera);
        // Draw UI
        Render(scene);

        ImGui::End();

        // Draw UI stuff (TODO: get a loop for rendering UI in one line because we have virtual functions!)
        m_HierarchyPanel->Render(scene, renderer);
        m_InspectorPanel->Render(scene, renderer);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorPanel::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void EditorPanel::RenderMenuBar() {
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

    void EditorPanel::RenderFileBar() {
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

    void EditorPanel::RenderEditBar() {
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

    void EditorPanel::RenderViewBar() {
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

    void EditorPanel::RenderDebugBar() {
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

    void EditorPanel::RenderHelpBar() {
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

    void EditorPanel::DrawPerformanceProfile() {
        if (Input::IsKeyPressed(REAL_KEY_F11)) openPerfProfile = !openPerfProfile;
        if (openPerfProfile) return;
        const auto fps = "FPS: " + std::to_string(Services::GetEditorTimer()->GetFPS());
        ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), fps.c_str());
    }

    void EditorPanel::UpdateInputUI() {
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            Services::GetEditorState()->selectedEntity = nullptr;
        }

        if (Input::IsKeyPressed(REAL_KEY_F3)) {
            Services::GetEditorState()->FpsMode = !Services::GetEditorState()->FpsMode;
        }
    }

    void EditorPanel::InitFontStyle() {
        // Font style
        // Hardcoded for now!!
        const auto& assetManager = Services::GetAssetManager();
        const auto assets_dir = String(ASSETS_DIR);

        const ImGuiIO& io = ImGui::GetIO();
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Regular",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                16.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Regular-Big",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                17.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Bold",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                16.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Bold-Big",
                io.Fonts->AddFontFromFileTTF(fontFile.c_str(),
                17.5f,
                nullptr,
                io.Fonts->GetGlyphRangesDefault())
            );
        }
    }

    void EditorPanel::InitDarkTheme() {
        // TODO: Background of text colors can be change
        ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(0.1568, 0.6294, 0.1137, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1765, 0.2157, 0.2823, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.19, 0.07, 0.79, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.09, 0.07, 0.09, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(5.0, 1.0, 1.0, 0.87);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2620, 0.3250, 0.28260, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3712, 0.4035, 0.3907, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.03954, 0.03914, 0.03934, 1.0);
    }

    void EditorPanel::RenderSceneGizmos() {
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

        Entity entity = *editorState->selectedEntity;
        auto& tc = entity.GetComponentUnchecked<TransformComponent>();
        auto& camera = editorState->editorCamera->GetComponent<CameraComponent>().m_Camera;

        const bool hasCollider = entity.HasComponent<ColliderComponent>();
        const bool hasPhysics  = entity.HasComponent<RigidbodyComponent>();

        // Decide whether ENTITY transform is editable
        bool canEditEntityTransform = true;

        if (hasPhysics) {
            const auto& pb = entity.GetComponentUnchecked<RigidbodyComponent>();
            if (pb.type == physics::BodyType::Dynamic) {
                canEditEntityTransform = false;
            }
        }

        /*
         * PhysX rule:
         * A shape never exists in world space
         * A shape is always relative to its actor
         * For example: shapeLocalPose = PxTransform(localPosition, localRotation);
         *
         * Static collider -> (Real) own transform
         * Dynamic collider -> (PhysX) own transform
         * Colliders always lives in actor space, Gizmos always edits world space
        */

        // Build actor (entity) world matrix without scaling,
        // This is intentional, because PhysX does not allow scaling actors
        const math::Mat4 actorWorld = math::Mat4::Translate(tc.transform.position) * tc.transform.rotation.ToMat4();

        // Choose gizmo matrix
        math::Mat4 gizmoMatrix = tc.transform.GetModelMatrix(); // Use entity transform as default

        // If entity has a ColliderComponent, let Physx manages transform of entity's collider transform
        if (!m_EditEntityTransform && hasCollider) {
            const auto& cc = entity.GetComponentUnchecked<ColliderComponent>();

            // ImGuizmo only understands world matrices
            gizmoMatrix = actorWorld * math::Mat4::Translate(cc.localPosition) * cc.localRotation.ToMat4();
        }

        // Draw gizmo
        ImGuizmo::Manipulate(camera.GetView().ValuePtr(), camera.GetProjection().ValuePtr(),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, gizmoMatrix.ValuePtr()
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

            tc.transform.SetPosition(translation);
            tc.transform.SetRotation(rotation);
            tc.transform.SetScale(scale);
            return;
        }

        // COLLIDER LOCAL TRANSFORM EDIT
        auto& cc = entity.GetComponentUnchecked<ColliderComponent>();
        if (cc.shape == physics::ColliderShape::Box) {
        }

        const math::Mat4 invActorWorld = actorWorld.Inverted();
        math::Mat4 localMatrix  = invActorWorld * gizmoMatrix;

        math::Vec3 localPos{}, localScale{};
        math::Quat localRot{};
        math::DecomposeTransform(localMatrix, localPos, localRot, localScale);

        cc.localPosition = localPos;
        cc.localRotation = localRot;
        // PhysX works in half extents, Debug mesh works in full extents,
        // in this case scale with 0.5 before sending to PhysX
        cc.size = localScale * 0.5f; // full -> half extents
    }

    void EditorPanel::UpdateGizmoLogic() {
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

    void EditorPanel::DebugGizmos() {
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

    void EditorPanel::Render(Scene* scene) {
        RenderMenuBar();
        RenderSceneGizmos();
        DrawPerformanceProfile();
        // DebugGizmos();
    }

}
