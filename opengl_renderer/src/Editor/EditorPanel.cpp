//
// Created by pointerlost on 10/17/25.
//
#include "Editor/EditorPanel.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/file_manager.h"
#include "Core/Services.h"
#include "Core/Window.h"
#include "Editor/EditorState.h"
#include "Editor/HierarchyPanel.h"
#include "Editor/InspectorPanel.h"
#include "Graphics/Renderer.h"
#include "Input/Keycodes.h"
#include "ImGuizmo/ImSequencer.h"
#include "ImGuizmo/ImCurveEdit.h"
#include "ImGuizmo/GraphEditor.h"
#include "Input/Input.h"
#include "Math/Math.h"
#include "Scene/Components.h"

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
        // ImGui::ShowDemoWindow();
    }

    void EditorPanel::Render(Scene* scene, opengl::Renderer* renderer) {
        // Scene window props
        // Hierarchy,Inspector width = SCREEN_WIDTH / 5 + 31.0
        // MenuBarPanel height = 25.0
        ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH / 5 + 31.0, 25.0));
        ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH - (SCREEN_WIDTH / 5 + 31.0) * 2, SCREEN_HEIGHT));

        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        renderer->Render(&Services::GetEditorState()->camera);
        m_HierarchyPanel->Render(scene, renderer);
        m_InspectorPanel->Render(scene, renderer);
        Render();
        ImGui::End();

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

        if (ImGui::BeginMenu("File", true)) {

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
    }

    void EditorPanel::UpdateInputUI() {
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            Services::GetEditorState()->selectedEntity = Entity{};
        }

        wasItemActivated = ImGui::IsItemActivated();

        if (wasItemActivated && ImGui::IsMouseDown(ImGuiMouseButton_Left) || Input::IsKeyPressed(REAL_KEY_F4)) {
            Services::GetEditorState()->fpsMode = false;
        } else {
            Services::GetEditorState()->fpsMode = true;
        }
    }

    void EditorPanel::InitFontStyle() {
        // Font style
        // Hardcoded for now!!
        const ImGuiIO& io = ImGui::GetIO();
        if (constexpr auto fontFile = "assets/fonts/Ubuntu/Ubuntu-Regular.ttf"; File::Exists(fontFile)) {
            m_Fonts["Ubuntu-Regular"] = io.Fonts->AddFontFromFileTTF(fontFile, 16.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
        }
        if (constexpr auto fontFile = "assets/fonts/Ubuntu/Ubuntu-Bold.ttf"; File::Exists(fontFile)) {
            m_Fonts["Ubuntu-Bold"] = io.Fonts->AddFontFromFileTTF(fontFile, 16.5f, nullptr, io.Fonts->GetGlyphRangesDefault());
        }
    }

    void EditorPanel::InitDarkTheme() {
        ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(0.1568, 0.6294, 0.1137, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1765, 0.2157, 0.2823, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.19, 0.07, 0.79, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.09, 0.07, 0.09, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(5.0, 1.0, 1.0, 0.87);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.1019, 0.1568, 0.1372, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2620, 0.3250, 0.28260, 1.0);
        ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3712, 0.4035, 0.3907, 1.0);
    }

    void EditorPanel::DrawGizmos() {
        if (Input::IsKeyPressed(REAL_KEY_E)) {
            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
        } else if (Input::IsKeyPressed(REAL_KEY_R)) {
            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
        } else if (Input::IsKeyPressed(REAL_KEY_T)){
            m_GizmoType = ImGuizmo::OPERATION::SCALE;
        }

        // Using perspective camera
        if (Services::GetEditorState()->selectedEntity) {
            // Using perspective projection
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            // Draw gizmos rect
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

            auto& transform = Services::GetEditorState()->selectedEntity.GetComponent<TransformComponent>()->m_Transform;
            auto& camera = Services::GetEditorState()->camera.GetComponent<CameraComponent>()->m_Camera;
            auto model = transform.GetModelMatrix();

            ImGuizmo::Manipulate(glm::value_ptr(camera.GetView()), glm::value_ptr(camera.GetProjection()),
                m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(model)
            );

            if (ImGuizmo::IsUsing()) {
                glm::vec3 translation, scale;
                glm::quat rotate;

                Math::DecomposeTransform(model, translation, rotate, scale);
                transform.SetTranslate(translation);
                transform.SetRotation(rotate);
                transform.SetScale(scale);
            }
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

    void EditorPanel::Render() {
        UpdateInputUI();
        RenderMenuBar();
        DrawPerformanceProfile();
        DrawGizmos();
        // DebugGizmos();
    }

    ImFont* EditorPanel::GetFontStyle(const std::string& fontName) {
        if (m_Fonts.contains(fontName)) {
            return m_Fonts[fontName];
        }
        Warn(ConcatStr("Font doesn't exists! from: ", __FILE__, "\n name: ", fontName));
        return nullptr;
    }
}
