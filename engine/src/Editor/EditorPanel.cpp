//
// Created by pointerlost on 10/17/25.
//
#include "Editor/EditorPanel.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "PxShape.h"
#include "Core/AssetManager.h"
#include "Core/file_manager.h"
#include "Core/Services.h"
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Editor/EditorState.h"
#include "Editor/HierarchyPanel.h"
#include "Editor/InspectorPanel.h"
#include "geometry/PxBoxGeometry.h"
#include "Graphics/Renderer.h"
#include "Input/Keycodes.h"
#include "ImGuizmo/ImSequencer.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "ImGuizmo/GraphEditor.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Scene/Components.h"
#include "Util/Util.h"

namespace Real::UI {

    EditorPanel::EditorPanel(Graphics::Window *window, HierarchyPanel* hierarchyPanel, InspectorPanel* inspectorPanel)
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

    void EditorPanel::Render(Scene* scene, opengl::Renderer* renderer) {
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
        renderer->Render(Services::GetEditorState()->camera);
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

        if (ImGui::BeginMenu("File")) {
            if (ImGui::Button("Bla bla")) {
            }
            if (ImGui::Button("Bla bla 2")) {
            }
            if (ImGui::Button("Bla bla 3")) {
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();
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
        const auto assets_dir = std::string(ASSETS_DIR);

        const ImGuiIO& io = ImGui::GetIO();
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Regular", io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 16.5f, nullptr, io.Fonts->GetGlyphRangesDefault()));
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Regular.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Regular-Big", io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 17.5f, nullptr, io.Fonts->GetGlyphRangesDefault()));
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Bold", io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 16.5f, nullptr, io.Fonts->GetGlyphRangesDefault()));
        }
        if (const auto fontFile = assets_dir + "fonts/Ubuntu/Ubuntu-Bold.ttf"; fs::File::Exists(fontFile)) {
            assetManager->AddFontStyle("Ubuntu-Bold-Big", io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 17.5f, nullptr, io.Fonts->GetGlyphRangesDefault()));
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

    void EditorPanel::DrawGizmos() {
        static bool transformGizmosOn = true;

        // Toggle gizmo target (entity <-> collider)
        if (Input::IsKeyPressed(REAL_KEY_Y))
            transformGizmosOn = !transformGizmosOn;

        // Select gizmo operation
        if (Input::IsKeyPressed(REAL_KEY_E))
            m_GizmoType = ImGuizmo::TRANSLATE;
        else if (Input::IsKeyPressed(REAL_KEY_R))
            m_GizmoType = ImGuizmo::ROTATE;
        else if (Input::IsKeyPressed(REAL_KEY_T))
            m_GizmoType = ImGuizmo::SCALE;

        const auto* editorState = Services::GetEditorState();
        if (!editorState->selectedEntity)
            return;

        // Currently using 3D
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(
            ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y,
            ImGui::GetWindowSize().x,
            ImGui::GetWindowSize().y
        );

        auto& entity    = *editorState->selectedEntity;
        auto& transform = entity.GetComponentUnchecked<TransformComponent>().transform;
        auto& collider  = entity.GetComponent<ColliderComponent>();
        auto& camera    = editorState->camera->GetComponent<CameraComponent>().m_Camera;

        // Build ACTOR world matrix (no scale!)
        const math::Mat4 actorWorld = math::Mat4::Translate(transform.position) * transform.rotation.ToMat4();

        // Build COLLIDER world matrix (actor x local)
        const math::Mat4 colliderWorld =
            actorWorld *
            math::Mat4::Translate(collider.localPosition) *
            collider.localRotation.ToMat4();

        // Choose which matrix the gizmo edits
        math::Mat4 gizmoMatrix = transformGizmosOn && entity.HasComponent<ColliderComponent>()
            ? transform.GetModelMatrix()
            : colliderWorld;

        ImGuizmo::Manipulate(camera.GetView().ValuePtr(), camera.GetProjection().ValuePtr(),
            (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, gizmoMatrix.ValuePtr()
        );

        if (!ImGuizmo::IsUsing())
            return;

        math::Vec3 translation{}, scale{};
        math::Quat rotation{};
        // Decompose manipulated matrix
        math::DecomposeTransform(gizmoMatrix, translation, rotation, scale);

        // ENTITY transform edited
        if (transformGizmosOn) {
            transform.SetPosition(translation);
            transform.SetRotation(rotation);
            transform.SetScale(scale);
            return;
        }

        // COLLIDER edited (convert WORLD to LOCAL)
        const math::Mat4 invActorWorld = actorWorld.Inverted();
        math::Mat4 localMatrix   = invActorWorld * gizmoMatrix;

        math::Vec3 localPos{}, localScale{};
        math::Quat localRot{};
        math::DecomposeTransform(localMatrix, localPos, localRot, localScale);

        collider.localPosition = localPos;
        collider.localRotation = localRot;
        collider.size          = localScale * 0.5f; // full -> half extents

        // Sync PhysX shape immediately
        if (collider.shapeHandle) {
            collider.shapeHandle->setLocalPose(
                physx::PxTransform(
                    util::RealToPX(collider.localPosition),
                    util::RealToPX(collider.localRotation)
                )
            );

            collider.shapeHandle->setGeometry(
                physx::PxBoxGeometry(
                    collider.size.x,
                    collider.size.y,
                    collider.size.z
                )
            );
        }
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
        UpdateInputUI();
        RenderMenuBar();
        DrawPerformanceProfile();
        DrawGizmos();
        // DebugGizmos();
    }

}
