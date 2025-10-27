//
// Created by pointerlost on 10/3/25.
//
#include "Core/Engine.h"
#include <Core/Config.h>
#include <Core/CmakeConfig.h>
#include <Core/Config.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

        // Editor camera2
        auto& camera2 = m_Scene->CreateEntity("Editor Camera2");
        (void)camera2.AddComponent<CameraComponent>();
        (void)camera2.AddComponent<VelocityComponent>();
        camera2.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-10.0, 2.0, 5.0));

        // Editor camera3
        auto& camera3 = m_Scene->CreateEntity("Editor Camera3");
        (void)camera3.AddComponent<CameraComponent>();
        (void)camera3.AddComponent<VelocityComponent>();
        camera3.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(10.0, 2.0, 5.0));

        m_CameraInput = CreateScope<CameraInput>(m_EditorState->camera);

        const auto& defaultMat = m_AssetManager->GetDefaultMat();
        const auto& material = Services::GetAssetManager()->CreateMaterialInstance("material");
        material->AddTexture(TextureType::BaseColor, m_AssetManager->GetTexture("floor_wood"));
        // material->AddTexture(TextureType::Specular, m_AssetManager->GetTexture("container2_specular"));

        const auto& material2 = Services::GetAssetManager()->CreateMaterialInstance("material2");
        material2->AddTexture(TextureType::BaseColor, m_AssetManager->GetTexture("container2"));
        material2->AddTexture(TextureType::Specular, m_AssetManager->GetTexture("container2_specular"));

        auto& cube = m_Scene->CreateEntity("RightWall");
        cube.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(26.0, 1.5, 0.0));
        cube.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(45.0, 20.0, 1.0));
        cube.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube.AddComponent<MaterialComponent>().m_Instance = material;

        auto& cube2 = m_Scene->CreateEntity("LeftWall");
        cube2.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-26.0, 1.5, 0.0));
        cube2.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(45.0, 20.0, 1.0));
        cube2.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube2.AddComponent<MaterialComponent>().m_Instance = material;

        auto& cube3 = m_Scene->CreateEntity("Floor");
        cube3.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(97.0, 0.5, 98.0));
        cube3.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube3.AddComponent<MaterialComponent>().m_Instance = material;

        auto& cube4 = m_Scene->CreateEntity("Roof");
        cube4.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 13.5, 0.0));
        cube4.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(97.0, 4.0, 1.0));
        cube4.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube4.AddComponent<MaterialComponent>().m_Instance = material;

        auto& cube5 = m_Scene->CreateEntity("Container");
        cube5.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 0.0, 12.0));
        cube5.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(8.0, 8.0, 8.0));
        cube5.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube5.AddComponent<MaterialComponent>().m_Instance = material2;

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-10.0, 10.0, -10.0));
        light.AddComponent<MeshComponent>().m_MeshName = "cube";
        light.AddComponent<LightComponent>().m_Light = Light{LightType::SPOT};
        light.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        m_Renderer->GetRenderContext()->InitResources();
        Info("Resources loaded successfully!");
    }

    void Engine::ShutDown() {
        glfwTerminate();
        // Cleanup Dear ImGui context
        m_EditorPanel->Shutdown();
    }

    void Engine::StartPhase() const {
        // Callbacks
        glfwPollEvents();
        glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Init UI
        m_EditorPanel->BeginFrame();
    }

    void Engine::EndPhase(GLFWwindow* window) {
        glfwSwapBuffers(window);
    }

    void Engine::RenderPhase() const {
        // Draw OpenGL stuff
        m_EditorPanel->Render(m_Scene.get(), m_Renderer.get());
    }

    void Engine::UpdatePhase() const {
        m_EditorTimer->Update();
        Input::Update(m_CameraInput.get());
        m_Systems->UpdateAll(m_Scene.get(), m_EditorTimer->GetDelta());
        m_Scene->Update(m_Renderer.get());
    }

    void Engine::InitServices() const {
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

    void Engine::SetOpenGLStateFunctions() {
        /*
        TODO:
            when i learn the different rendering techniques, (e.g. more advanced deferred rendering)
            need update to apply gamma correction in CPU, for now we will do in GPU-side
        */
        // Activate automatic Gamma Correction
        // glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_ALWAYS);

        // This only has affect if depth testing is enabled
        // glDepthMask(GL_FALSE);
    }

    void Engine::Running() {
        const auto window = m_Window->GetGLFWWindow();
        glfwSwapInterval(0);

        SetOpenGLStateFunctions();
        while (!glfwWindowShouldClose(window) && !Input::IsKeyPressed(REAL_KEY_ESCAPE)) {
            StartPhase();
            UpdatePhase();
            RenderPhase();
            EndPhase(window);
        }
    }
}
