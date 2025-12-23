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
        m_AssetImporter.reset();
        ShutDown();
    }

    void Engine::InitResources() {
        // The order is matter!
        InitWindow();
        InitCallbacks(m_Window->GetGLFWWindow());
        InitSystems();
        InitAssetImporter();
        InitAssetManager();
        InitMeshManager();

        // Editor state
        InitEditorState();
        InitEditorScene();
        InitEditorRenderer();

        InitServices();

        InitEditorUIState();
        InitEditorCamera();

        // Load all the resources with the ResourceLoader
        InitResourceLoader();

        Info("Engine Resources loaded successfully!");
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

    void Engine::InitWindow() {
        m_Window = CreateScope<Graphics::Window>(SCREEN_WIDTH, SCREEN_HEIGHT, "Human consciousness");
        m_Window->Init();
        Info("Window initialized successfully!");
    }

    void Engine::RenderPhase() const {
        // Draw OpenGL stuff
        m_EditorPanel->Render(m_Scene.get(), m_Renderer.get());
        // TODO: Requires double buffering to switch between each other (Thread-safe rendering and to keep sync CPU-GPU)
    }

    void Engine::UpdatePhase() const {
        m_EditorTimer->Update();
        Input::Update(m_CameraInput.get());
        m_AssetImporter->Update();
        m_AssetManager->Update();
        m_Systems->UpdateAll(m_Scene.get(), m_EditorTimer->GetDelta());
        m_Scene->Update(m_Renderer.get());
    }

    void Engine::InitServices() const {
        Services::SetAssetManager(m_AssetManager.get());
        Services::SetMeshManager(m_MeshManager.get());
        Services::SetEditorTimer(m_EditorTimer.get());
        Services::SetEditorState(m_EditorState.get());
        Services::SetAssetImporter(m_AssetImporter.get());
        // TODO: Need Shader manager?

        Info("Services initialized successfully!");
    }

    void Engine::InitSystems() {
        m_Systems = CreateScope<Systems>();
        // Init sub-systems
        m_Systems->Init();
        Info("Systems initialized successfully!");
    }

    void Engine::InitAssetImporter() {
        m_AssetImporter = CreateScope<AssetImporter>();
        Info("Asset Importer initialized successfully!");
    }

    void Engine::InitEditorState() {
        m_EditorTimer = CreateScope<Timer>();
        m_EditorTimer->Start();
        m_EditorState = CreateScope<EditorState>();
        Info("Editor State initialized successfully!");
    }

    void Engine::InitEditorScene() {
        m_Scene = CreateScope<Scene>();
        Info("Editor Scene initialized successfully!");
    }

    void Engine::InitEditorRenderer() {
        m_Renderer = CreateScope<opengl::Renderer>(m_Scene.get());
        Info("Editor Renderer initialized successfully!");
    }

    void Engine::InitEditorUIState() {
        // The order is matter!!!
        m_HierarchyPanel = CreateScope<UI::HierarchyPanel>();
        m_InspectorPanel = CreateScope<UI::InspectorPanel>();
        m_EditorPanel    = CreateScope<UI::EditorPanel>(m_Window.get(), m_HierarchyPanel.get(), m_InspectorPanel.get());
        Info("EditorPanel initialized successfully!");
    }

    void Engine::InitEditorCamera() {
        // Editor camera
        m_EditorState->camera = &m_Scene->CreateEntity("Editor Camera");
        (void)m_EditorState->camera->AddComponent<CameraComponent>();
        (void)m_EditorState->camera->AddComponent<VelocityComponent>();
        m_EditorState->camera->GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 2.0, 5.0));

        if (m_EditorState->camera) {
            m_CameraInput = CreateScope<CameraInput>(m_EditorState->camera);
        } else {
            Warn("There is no camera in editor state!!!");
        }
        Info("Editor Camera and Camera input initialized successfully!");
    }

    void Engine::InitResourceLoader() {
        m_ResourceLoader = CreateScope<ResourceLoader>(m_Renderer->GetRenderContext());
        m_ResourceLoader->Load();
        Info("Resource loader initialized successfully!");
    }

    void Engine::InitAssetManager() {
        m_AssetManager = CreateScope<AssetManager>();
        Info("AssetManager initialized successfully!");
    }

    void Engine::InitMeshManager() {
        m_MeshManager = CreateScope<MeshData>();
        m_MeshManager->InitResources();
        Info("MeshManager initialized successfully!");
    }

    void Engine::SetOpenGLStateFunctions() {
        /*
        TODO:
            When i learn the different rendering techniques, (e.g. more advanced deferred rendering)
            need update to apply gamma correction in CPU (is it worth?), for now we will do in GPU-side
        */
        // Activate automatic Gamma Correction
        // glEnable(GL_FRAMEBUFFER_SRGB);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEBUG_OUTPUT);
        // glDepthFunc(GL_LEQUAL);
        // glDepthFunc(GL_EQUAL);
        // glEnable(GL_STENCIL_TEST);

        // This only has affect if depth testing is enabled
        // glDepthMask(GL_FALSE);
    }

    void Engine::InitGameResources() {
        const auto& cube = m_Scene->CreateEntity("RightWall");
        cube.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(26.0, 1.5, 0.0));
        cube.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(45.0, 20.0, 1.0));
        cube.GetComponent<MeshRendererComponent>()->m_MeshID = Services::GetMeshManager()->GetPrimitiveUUID("cube");

        const auto& cube2 = m_Scene->CreateEntity("LeftWall");
        cube2.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-26.0, 1.5, 0.0));
        cube2.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(45.0, 20.0, 1.0));
        cube2.GetComponent<MeshRendererComponent>()->m_MeshID = Services::GetMeshManager()->GetPrimitiveUUID("cube");

        const auto& cube3 = m_Scene->CreateEntity("Floor");
        cube3.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(97.0, 0.5, 98.0));
        cube3.GetComponent<MeshRendererComponent>()->m_MeshID = Services::GetMeshManager()->GetPrimitiveUUID("cube");

        const auto& cube4 = m_Scene->CreateEntity("Roof");
        cube4.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 13.5, 0.0));
        cube4.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(97.0, 4.0, 1.0));
        cube4.GetComponent<MeshRendererComponent>()->m_MeshID = Services::GetMeshManager()->GetPrimitiveUUID("cube");

        const auto& cube5 = m_Scene->CreateEntity("Container");
        cube5.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(0.0, 0.0, 12.0));
        cube5.GetComponent<TransformComponent>()->m_Transform.SetScale(glm::vec3(8.0, 8.0, 8.0));
        cube5.GetComponent<MeshRendererComponent>()->m_MeshID = Services::GetMeshManager()->GetPrimitiveUUID("cube");

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponent<TransformComponent>()->m_Transform.SetTranslate(glm::vec3(-10.0, 10.0, -10.0));
        light.AddComponent<LightComponent>().m_Light = Light{LightType::POINT};
    }

    void Engine::Running() {
        const auto window = m_Window->GetGLFWWindow();
        glfwSwapInterval(0);

        SetOpenGLStateFunctions();
        while (!glfwWindowShouldClose(window) && !Input::IsKeyPressed(REAL_KEY_ESCAPE)) {
            StartPhase();
            UpdatePhase();
            RenderPhase(); // TODO: Thread-safe rendering
            EndPhase(window);
        }
    }
}
