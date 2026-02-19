//
// Created by pointerlost on 10/3/25.
//
#include "Core/Engine.h"

#include <GLFW/glfw3.h>

#include "Core/Callback.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "Physics/Physx/PhysXBackend.h"
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
        m_DebugRenderer.reset();
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

        // Debugging state
        InitDebugRenderer();

        InitServices();

        InitEditorUIState();
        InitEditorCamera();

        // Load all the resources with the ResourceLoader
        InitResourceLoader();

        Info("Engine Resources loaded successfully!");
    }

    void Engine::ShutDown() {
        // Cleanup Dear ImGui context
        m_EditorPanel->Shutdown();
    }

    void Engine::StartPhase() const {
        // Callbacks
        glfwPollEvents();
        glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Init UI
        m_EditorPanel->BeginFrame(m_EditorTimer->GetDelta());
        m_DebugRenderer->BeginFrame();
    }

    void Engine::UpdatePhase() const {
        m_EditorTimer->Update();
        m_EditorPanel->Update(m_EditorTimer->GetDelta());
        Input::Update(m_CameraInput.get());
        m_AssetImporter->Update();
        m_AssetManager->Update();
        m_Systems->Update(m_Scene.get(), m_EditorTimer->GetDelta());
        m_Scene->Update(m_Renderer.get());
        m_DebugRenderer->Update();
    }

    void Engine::RenderPhase() const {
        // Draw OpenGL stuff
        m_DebugRenderer->Render();
        // Main Scene window
        m_Renderer->Render(Services::GetEditorState()->editorCamera);
        m_EditorPanel->RenderUI();

        // TODO: Requires double buffering to switch between each other (Thread-safe rendering and to keep sync CPU-GPU)
    }

    void Engine::EndPhase() {
        m_EditorPanel->EndFrame();
        m_DebugRenderer->EndFrame();
    }

    void Engine::SetOpenGLStateFunctions() {
        /*
        TODO:
            When i learn the different rendering techniques, (e.g. more advanced deferred rendering)
            need update to apply gamma correction in CPU (is it worth?), for now we will do in GPU-side
        */
        // Activate automatic Gamma Correction
        // glEnable(GL_FRAMEBUFFER_SRGB);

        // TODO: need an update for drawing opengl lines (depth testing etc.)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEBUG_OUTPUT);
        // glDepthFunc(GL_LEQUAL);
        // glDepthFunc(GL_EQUAL);
        // glEnable(GL_STENCIL_TEST);

        // This only has affect if depth testing is enabled
        // glDepthMask(GL_FALSE);
    }

    void Engine::AttachSceneToSystems() {

        Info("AttachSceneToSystems initialized successfully!");
    }

    void Engine::SetActiveScene(Scene *scene) {
        // TODO: fix it, scene is unique_ptr
        // m_Scene = scene;

        m_Systems->OnSceneAttach(scene);
    }

    void Engine::InitGameResources() {
        auto& cube = m_Scene->CreateEntity("RightWall");
        cube.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(26.0, 1.5, 0.0));
        cube.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(45.0, 20.0, 1.0));
        (void)cube.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube2 = m_Scene->CreateEntity("LeftWall");
        cube2.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(-26.0, 1.5, 0.0));
        cube2.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(45.0, 20.0, 1.0));
        (void)cube2.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube3 = m_Scene->CreateEntity("Floor");
        cube3.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(97.0, 1.5, 98.0));
        (void)cube3.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );
        cube3.AddComponent<ColliderComponent>().shape = physics::ColliderShape::Box;
        cube3.AddComponent<RigidbodyComponent>().type = physics::BodyType::Static;

        auto& cube4 = m_Scene->CreateEntity("Roof");
        cube4.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 13.5, 0.0));
        cube4.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(97.0, 4.0, 1.0));
        (void)cube4.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        auto& cube5 = m_Scene->CreateEntity("Container");
        cube5.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 0.0, 12.0));
        cube5.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(8.0, 8.0, 8.0));
        (void)cube5.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );
        cube5.AddComponent<ColliderComponent>().shape = physics::ColliderShape::Box;
        cube5.AddComponent<RigidbodyComponent>().type = physics::BodyType::Static;

        auto& fordCar = m_Scene->CreateEntity("FordCar");
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        fordCar.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)fordCar.AddComponent<ModelComponent>(m_AssetManager->GetModel("Ford_raptor"));

        auto& island_tree = m_Scene->CreateEntity("Island Tree");
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        island_tree.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)island_tree.AddComponent<ModelComponent>(m_AssetManager->GetModel("island_tree"));

        auto& mountain_road = m_Scene->CreateEntity("Mountain road");
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        mountain_road.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)mountain_road.AddComponent<ModelComponent>(m_AssetManager->GetModel("mountain_road"));

        auto& porsche_car = m_Scene->CreateEntity("Porsche");
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        porsche_car.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)porsche_car.AddComponent<ModelComponent>(m_AssetManager->GetModel("porsche_turbo"));

        auto& city_road = m_Scene->CreateEntity("City Road");
        city_road.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(0.0, 10.0, 0.0));
        city_road.GetComponentForModification<TransformComponent>()->transform.SetScale(math::Vec3(1.0, 1.0, 1.0));
        (void)city_road.AddComponent<ModelComponent>(m_AssetManager->GetModel("city_road"));

        auto& light = m_Scene->CreateEntity("Light");
        light.GetComponentForModification<TransformComponent>()->transform.SetPosition(math::Vec3(-10.0, 10.0, -10.0));
        (void)light.AddComponent<LightComponent>(Light::Mode::DIRECTIONAL);
        (void)light.AddComponent<MeshRendererComponent>(Services::GetMeshManager()->GetPrimitiveUUID("cube"),
            Services::GetAssetManager()->CreateMaterialInstance("Marble009")
        );

        Info("Game resources loaded successfully!");
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
