//
// Created by pointerlost on 10/3/25.
//
#include "Core/Engine.h"
#include <Core/Config.h>
#include "Core/Callback.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Transformations.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "Scene/Components.h"

namespace Real {

    Engine::~Engine() {
        m_Scene.reset();
        m_CameraInput.reset();
        m_AssetManager.reset();
        m_Renderer.reset();
        m_MeshManager.reset();
        m_EditorTimer.reset();
        m_Window.reset();
        m_EditorState.reset();
        ShutDown();
    }

    void Engine::InitResources() {
        m_Window = CreateScope<Graphics::Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Human consciousness");
        m_Window->Init();
        InitCallbacks(m_Window->GetGLFWWindow());

        m_EditorTimer = CreateScope<Timer>();
        m_EditorTimer->Start();
        m_EditorState = CreateScope<EditorState>();
        // Init UI (the order is matter!!!)
        m_EditorPanel = CreateScope<UI::EditorPanel>(m_Window.get());
        m_HierarchyPanel = CreateScope<UI::HierarchyPanel>(m_EditorPanel.get());
        m_InspectorPanel = CreateScope<UI::InspectorPanel>(m_EditorPanel.get());

        InitAsset();
        InitMesh();
        InitServices();

        const auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        const auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        Services::GetAssetManager()->LoadShader(vert, frag, "main");

        m_MeshManager->InitResources();

        // m_AssetManager->LoadTexture("assets/textures/container.jpg", "container");
        // m_AssetManager->LoadTexture("assets/textures/container.jpg", "container2");

        m_Scene = CreateScope<Scene>();
        m_Renderer = CreateScope<opengl::Renderer>(m_Scene.get());

        m_EditorState->camera = m_Scene->CreateEntity("Editor Camera");
        (void)m_EditorState->camera.AddComponent<CameraComponent>();
        m_EditorState->camera.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 0.0, 5.0));

        m_CameraInput = CreateScope<CameraInput>(&m_EditorState->camera);

        const auto& defaultMat = Services::GetAssetManager()->GetDefaultMat();

        auto& cube = m_Scene->CreateEntity("Cube");
        cube.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0));
        cube.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(4.0));
        cube.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        auto& cube2 = m_Scene->CreateEntity("Cube2");
        cube2.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(10.0, 4.0, 0.0));
        cube2.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(8.0));
        cube2.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube2.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-4.0, 4.0, -2.0));
        light.AddComponent<MeshComponent>().m_MeshName = "cube";
        light.AddComponent<LightComponent>().m_Light = Light{LightType::DIRECTIONAL};
        light.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        m_AssetManager->LoadTextures();
        m_Renderer->GetRenderContext()->InitResources();
        Info("Resources loaded successfully!");
    }

    void Engine::ShutDown() {
        glfwTerminate();
        // Cleanup Dear ImGui context
        m_EditorPanel->Shutdown();
    }

    void Engine::StartPhase() {
        // Callbacks
        glfwPollEvents();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Init UI
        m_EditorPanel->BeginFrame();
    }

    void Engine::EndPhase(GLFWwindow* window) {
        glfwSwapBuffers(window);
    }

    void Engine::RenderPhase() {
        // Draw OpenGL stuff
        m_Renderer->Render(&m_EditorState->camera);
        // Draw UI stuff (TODO: get a loop for rendering UI in one line because we have virtual functions!)
        m_InspectorPanel->Render(m_Scene.get());
        m_HierarchyPanel->Render(m_Scene.get());
        m_EditorPanel->Render(m_Scene.get());
    }

    void Engine::UpdatePhase() {
        m_EditorTimer->Update();
        Input::Update(m_CameraInput.get());
        m_Scene->Update();
    }

    void Engine::InitServices() {
        Services::SetAssetManager(m_AssetManager.get());
        Services::SetMeshManager(m_MeshManager.get());
        Services::SetEditorTimer(m_EditorTimer.get());
        Services::SetEditorState(m_EditorState.get());
    }

    void Engine::InitAsset() {
        m_AssetManager = CreateScope<AssetManager>();
        Info("AssetManager initialized successfully!");
    }

    void Engine::InitMesh() {
        m_MeshManager = CreateScope<MeshManager>();
        Info("MeshManager initialized successfully!");
    }

    void Engine::Running() {
        const auto window = m_Window->GetGLFWWindow();
        while (!glfwWindowShouldClose(window) && !Input::IsKeyPressed(REAL_KEY_ESCAPE)) {
            StartPhase();
            UpdatePhase();
            RenderPhase();
            EndPhase(window);
        }
    }
}
