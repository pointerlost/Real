//
// Created by pointerlost on 10/3/25.
//
#include "Core/Engine.h"
#include <Core/Config.h>
#include "Core/Callback.h"
#include "Core/file_manager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
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

        m_Systems = CreateScope<Systems>();
        // Init sub-systems
        m_Systems->Init();

        m_EditorTimer = CreateScope<Timer>();
        m_EditorTimer->Start();
        m_EditorState = CreateScope<EditorState>();

        InitAsset();
        InitMesh();
        InitServices();

        // Init UI (the order is matter!!!)
        m_HierarchyPanel = CreateScope<UI::HierarchyPanel>();
        m_InspectorPanel = CreateScope<UI::InspectorPanel>();
        m_EditorPanel = CreateScope<UI::EditorPanel>(m_Window.get(), m_HierarchyPanel.get(), m_InspectorPanel.get());

        const auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        const auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        Services::GetAssetManager()->LoadShader(vert, frag, "main");

        m_MeshManager->InitResources();

        m_Scene = CreateScope<Scene>();
        m_Renderer = CreateScope<opengl::Renderer>(m_Scene.get());
        m_AssetManager->LoadTextures();

        m_EditorState->camera = &m_Scene->CreateEntity("Editor Camera");
        (void)m_EditorState->camera->AddComponent<CameraComponent>();
        (void)m_EditorState->camera->AddComponent<VelocityComponent>();
        m_EditorState->camera->GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 2.0, 5.0));

        m_CameraInput = CreateScope<CameraInput>(m_EditorState->camera);

        const auto& defaultMat = m_AssetManager->GetDefaultMat();
        const auto& material = Services::GetAssetManager()->CreateMaterialInstance("material");
        material->AddTexture(TextureType::BaseColor, m_AssetManager->GetTexture("container2"));
        material->AddTexture(TextureType::Specular, m_AssetManager->GetTexture("container2_specular"));

        auto& cube = m_Scene->CreateEntity("Cube");
        cube.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0));
        cube.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(30.0, 2.5, 30.0));
        cube.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube.AddComponent<MaterialComponent>().m_Instance = material;

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-10.0, 10.0, -10.0));
        light.AddComponent<MeshComponent>().m_MeshName = "cube";
        light.AddComponent<LightComponent>().m_Light = Light{LightType::DIRECTIONAL};
        light.AddComponent<MaterialComponent>().m_Instance = defaultMat;

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
        m_EditorPanel->Render(m_Scene.get(), m_Renderer.get());
    }

    void Engine::UpdatePhase() {
        m_EditorTimer->Update();
        Input::Update(m_CameraInput.get());
        m_Systems->UpdateAll(m_Scene.get(), m_EditorTimer->GetDelta());
        m_Scene->Update(m_Renderer.get());
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
        glfwSwapInterval(0);
        while (!glfwWindowShouldClose(window) && !Input::IsKeyPressed(REAL_KEY_ESCAPE)) {
            StartPhase();
            UpdatePhase();
            RenderPhase();
            EndPhase(window);
        }
    }
}
