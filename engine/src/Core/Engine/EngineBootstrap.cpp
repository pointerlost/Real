//
// Created by pointerlost on 2/17/26.
//
#include "Core/Engine/EngineBootstrap.h"

#include "Core/Window/GLFWwindow.h"
#include "../../../../apps/editor/include/Editor.h"
#include "Core/Services.h"
#include "Core/Window/GLFWPlatform.h"
#include "Graphics/ModelLoader.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Platform/opengl/OpenGLRenderDevice.h"
#include "Platform/opengl/OpenGLRenderer.h"
#include "Platform/vulkan/VkRenderDevice.h"
#include "Platform/vulkan/VkRenderer.h"
#include "Resource/ResourceLoader.h"
#include "Scene/Scene.h"
#include "Scene/Systems/CameraSystem.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/MovementSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Scene/Systems/SystemsManager.h"
#include "Timer/Timer.h"

namespace Real::core {

    Scope<EngineCore> EngineBootstrap::Build(const EngineConfig &cfg)
    {
        auto cs = BuildCoreSystems(cfg);
        auto as = BuildAssetSystems();

        return CreateScope<EngineCore>(cs, as);
    }

    CoreSystems EngineBootstrap::BuildCoreSystems(const EngineConfig &cfg) {
        CoreSystems cs;

        cs.window = CreateWindow(cfg);
        cs.platform = CreatePlatform(cfg);

        auto renderDevice = CreateRenderDevice(cfg, *cs.window);
        cs.renderer = CreateRenderer(std::move(renderDevice), cfg);

        cs.physicsBackend = CreatePhysicsBackend(cfg);

        cs.scene = CreateScope<Scene>();

        cs.systems = CreateScope<SystemManager>();
        RegisterSystems(cs.systems.get(), *cs.physicsBackend);
        cs.systems->OnSceneAttach(cs.scene->GetRegistry(), cs.scene->GetEvents());

        cs.timer = CreateScope<RealTimeTimer>();

        cs.debugRenderer = CreateScope<graphics::debug::DebugRenderer>();

        return cs;
    }

    AssetSystems EngineBootstrap::BuildAssetSystems() {
        AssetSystems as;

        as.assetManager   = CreateScope<AssetManager>();
        as.meshManager    = CreateScope<MeshManager>();
        as.resourceLoader = CreateScope<ResourceLoader>();
        as.assetImporter  = CreateScope<AssetImporter>();

        return as;
    }

    Scope<IWindow> EngineBootstrap::CreateWindow(const EngineConfig &cfg) {
        switch (cfg.windowConfig.type)
        {
            case WindowType::glfw:
                return CreateScope<platform::glfw::GLFWWindow>(cfg);
            case WindowType::sdl:
                throw std::runtime_error("SDL unsupported for now! Window can't initialized, change to GLFW");

            default:
                throw std::runtime_error("Unsupported Window library!");
        }
    }

    Scope<IPlatform> EngineBootstrap::CreatePlatform(const EngineConfig &cfg) {
        switch (cfg.windowConfig.type) {
            case WindowType::glfw:
                return CreateScope<platform::GLFWPlatform>();
            case WindowType::sdl:
                throw std::runtime_error("SDL unsupported for now! Platform can't initialized, change to GLFW");
                // return CreateScope<platform::GLFWPlatform>();
            default: ;
        }
    }

    Scope<IRenderDevice> EngineBootstrap::CreateRenderDevice(const EngineConfig &cfg, IWindow &window) {
        switch (cfg.apiType)
        {
            case API::OpenGL:
                return CreateScope<platform::opengl::OpenGLRenderDevice>(window.GetNativeHandle(), cfg.rendererConfig);

            case API::Vulkan:
                return CreateScope<platform::vk::VkRenderDevice>(window.GetNativeHandle(), cfg.rendererConfig);

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

    Scope<IRenderer> EngineBootstrap::CreateRenderer(Scope<IRenderDevice> graphicsBackend, const EngineConfig &cfg) {
        switch (cfg.rendererType)
        {
            case RendererType::OpenGL:
                return CreateScope<platform::opengl::OpenGLRenderer>(graphicsBackend);

            case RendererType::Vulkan:
                return CreateScope<platform::vk::VkRenderer>(graphicsBackend);

            default:
                throw std::runtime_error("Unsupported renderer type");
        }
    }

    void EngineBootstrap::RegisterSystems(SystemManager *sysMngr, physics::IPhysicsBackend& physicsBackend) {
        sysMngr->AddSystem(CreateScope<ecs::CameraSystem>());
        sysMngr->AddSystem(CreateScope<ecs::PhysicsSystem>(physicsBackend));
        sysMngr->AddSystem(CreateScope<ecs::MovementSystem>());
        sysMngr->AddSystem(CreateScope<ecs::MeshRendererSystem>());
        sysMngr->AddSystem(CreateScope<ecs::LightSystem>());

        sysMngr->Init();
    }
}
