//
// Created by pointerlost on 10/17/25.
//
#include "Editor/EditorPanel.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Core/Logger.h"
#include "Core/Window.h"
#include "Graphics/Config.h"

namespace Real::UI {

    EditorPanel::EditorPanel(Graphics::Window *window) : m_Window(window)
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
    }

    void EditorPanel::BeginFrame() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();
    }

    void EditorPanel::Render(Scene* scene) {
        // This function should be the center of the main loop
        // Render custom engine stuff with opengl
        // then render UI stuff with ImGui
        RenderMenuBar();
        DrawPerformanceProfile();

        // Call last
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // after the rendering ui stuff, swapBuffers etc.
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
}
