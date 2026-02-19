//
// Created by pointerlost on 2/17/26.
//
#include "Core/Engine/EngineBootstrap.h"

#include "Core/Window/GLFWwindow.h"
#include "../../../../apps/editor/include/Editor.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Platform/opengl/OpenGLBackend.h"
#include "Platform/opengl/OpenGLRenderer.h"
#include "Platform/vulkan/VkBackend.h"
#include "Platform/vulkan/VkRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Systems/CameraSystem.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/MovementSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Scene/Systems/SystemsManager.h"
#include "Timer/Timer.h"

namespace Real::core {

    Scope<EngineCore> EngineBootstrap::Build(const EngineConfig &cfg) {
        auto window = CreateWindow(cfg);
        auto graphicsBackend = CreateGraphicsBackend(cfg, *window);
        auto physicsBackend = CreatePhysicsBackend(cfg);
        auto scene = CreateScope<Scene>();

        auto systems = CreateScope<SystemManager>();
        RegisterSystems(systems.get(), std::move(physicsBackend));
        systems->OnSceneAttach(scene->GetRegistry(), scene->GetEvents());

        auto editorTimer = CreateScope<RealTimeTimer>();
        auto editor = CreateScope<UI::Editor>(window.get());

        auto renderer = CreateRenderer(cfg, *window);

        return CreateScope<EngineCore>(
            std::move(window),
            std::move(graphicsBackend),
            std::move(renderer),
            std::move(scene),
            std::move(systems),
            cfg.editorMode ? std::move(editor) : nullptr,
            std::move(editorTimer)
        );
    }

    Scope<IWindow> EngineBootstrap::CreateWindow(const EngineConfig &cfg) {
        switch (cfg.windowConfig.type)
        {
            case WindowType::glfw:
                return CreateScope<platform::glfw::GLFWWindow>(cfg);
            case WindowType::sdl:
                throw std::runtime_error("SDL unsupported for now!");

            default:
                throw std::runtime_error("Unsupported Window library!");
        }
    }

    Scope<IGraphicsBackend> EngineBootstrap::CreateGraphicsBackend(const EngineConfig &cfg, IWindow &window) {
        switch (cfg.apiType)
        {
            case API::OpenGL:
                return CreateScope<platform::opengl::OpenGLBackend>(window.GetNativeHandle(), cfg.rendererConfig);

            case API::Vulkan:
                return CreateScope<platform::vk::VkBackend>(window.GetNativeHandle(), cfg.rendererConfig);

            default:
                throw std::runtime_error("Unsupported graphics API!");
        }
    }

    Scope<physics::IPhysicsBackend> EngineBootstrap::CreatePhysicsBackend(const EngineConfig &cfg) {
        switch (cfg.physicsBackendType)
        {
            case PhysicsBackendType::PhysX:
                return CreateScope<physics::PhysXBackend>();

            // case PhysicsBackendType::Bullet:
            //     return CreateScope<physics::BulletBackend>();

            default:
                throw std::runtime_error("Unsupported physics backend!");
        }
    }

    Scope<IRenderer> EngineBootstrap::CreateRenderer(const EngineConfig &cfg, IWindow &window) {
        switch (cfg.rendererType)
        {
            case RendererType::OpenGL:
                return CreateScope<platform::opengl::OpenGLRenderer>();

            case RendererType::Vulkan:
                return CreateScope<platform::vk::VkRenderer>();

            default:
                throw std::runtime_error("Unsupported renderer type");
        }
    }

    void EngineBootstrap::RegisterSystems(SystemManager *sysMngr, Scope<physics::IPhysicsBackend> physicsBackend) {
        sysMngr->AddSystem(CreateScope<ecs::CameraSystem>());
        sysMngr->AddSystem(CreateScope<ecs::PhysicsSystem>(std::move(physicsBackend)));
        sysMngr->AddSystem(CreateScope<ecs::MovementSystem>());
        sysMngr->AddSystem(CreateScope<ecs::MeshRendererSystem>());
        sysMngr->AddSystem(CreateScope<ecs::LightSystem>());

        sysMngr->Init();
    }
}
