//
// Created by pointerlost on 10/3/25.
//
#include "Core/Engine.h"
#include <ranges>
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
        ShutDown();
    }

    void Engine::InitResources() {
        m_Window = CreateScope<Graphics::Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Sometimes we just need to do");
        m_Window->Init();
        InitCallbacks(m_Window->GetWindow());

        m_EditorTimer = CreateScope<Timer>();
        m_EditorTimer->Start();

        InitAsset();
        InitMesh();
        InitServices();

        const auto vert = ConcatStr(SHADERS_DIR, "opengl/main.vert");
        const auto frag = ConcatStr(SHADERS_DIR, "opengl/main.frag");
        Services::GetAssetManager()->LoadShader(vert, frag, "main");

        m_MeshManager->InitResourcesWithBufferData();

        // m_AssetManager->LoadTexture("assets/textures/container.jpg", "container");
        // m_AssetManager->LoadTexture("assets/textures/container.jpg", "container2");

        m_Scene = CreateScope<Scene>();
        m_Renderer = CreateScope<opengl::Renderer>(m_Scene.get());

        editorCamera = m_Scene->CreateEntity("Editor Camera");
        (void)editorCamera.AddComponent<CameraComponent>();
        editorCamera.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 0.0, 5.0f));

        m_CameraInput = CreateScope<CameraInput>(&editorCamera);

        const auto& defaultMat = Services::GetAssetManager()->GetDefaultMat();

        auto& cube = m_Scene->CreateEntity("Cube");
        cube.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(2.0, 2.0, 0.0f));
        cube.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        auto& cube2 = m_Scene->CreateEntity("Cube2");
        cube2.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(4.0, 2.0, 0.0f));
        cube2.AddComponent<MeshComponent>().m_MeshName = "cube";
        cube2.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(5.0, 4.0, 0.0));
        light.AddComponent<MeshComponent>().m_MeshName = "cube";
        light.AddComponent<LightComponent>().m_Light = Light{};
        light.AddComponent<MaterialComponent>().m_Instance = defaultMat;

        m_Renderer->GetRenderContext()->InitResources();
    }

    void Engine::ShutDown() {
        glfwTerminate();
    }

    void Engine::StartPhase() {
        // Callbacks
        glfwPollEvents();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Engine::EndPhase(GLFWwindow* window) {
        glfwSwapBuffers(window);
    }

    void Engine::RenderPhase() {
        m_Renderer->Render(&editorCamera);
    }

    void Engine::UpdatePhase() {
        m_EditorTimer->Update();
        Input::Update(m_CameraInput.get());
        m_Scene->Update(&editorCamera);
    }

    void Engine::InitServices() {
        Services::SetAssetManager(m_AssetManager.get());
        Services::SetMeshManager(m_MeshManager.get());
        Services::SetEditorTimer(m_EditorTimer.get());
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
        const auto window = m_Window->GetWindow();
        while (!glfwWindowShouldClose(window) && !Input::IsKeyPressed(REAL_KEY_ESCAPE)) {
            StartPhase();
            UpdatePhase();
            RenderPhase();
            EndPhase(window);
        }
    }
}
