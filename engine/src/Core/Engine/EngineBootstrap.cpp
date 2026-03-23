//
// Created by pointerlost on 2/17/26.
//
#include "Core/Engine/EngineBootstrap.h"
#include "Core/Engine/EngineCore.h"
#include "Core/IApplicationContext.h"
#include "../../../include/RHI/IRenderDevice.h"
#include "Core/Logger.h"
#include "Core/RealConfig.h"
#include "Core/Services.h"
#include "Core/Window/GLFWPlatform.h"
#include "Core/Window/GLFWwindow.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Platform/OpenGL/OpenGLRenderDevice.h"
#include "Platform/OpenGL/OpenGLRenderer.h"
#include "Platform/Vulkan/VkRenderDevice.h"
#include "Platform/Vulkan/VkRenderer.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/MovementSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Scene/Systems/CameraSystem.h"
#include "Graphics/Shader.h"
#include "Platform/GLFW/GLFWPlatform.h"
#include "Platform/GLFW/GLFWwindow.h"
#include "Window/IWindow.h"

namespace Real::core {

    Scope<EngineCore> EngineBootstrap::Build(const EngineConfig& cfg, Scope<IApplication> app)
    {
        auto cs = BuildCoreSystems(cfg);
        auto as = BuildAssetSystems();

        assert(cs->physicsBackend && "Physics backend failed to initialize");
        auto rs = RegisterSystems(cs->systems.get(), *cs->physicsBackend);

        PopulateContext(app->GetContext(), *cs, *as, rs);

        return CreateScope<EngineCore>(std::move(cs), std::move(as), std::move(app));
    }

    Scope<CoreSystems> EngineBootstrap::BuildCoreSystems(const EngineConfig &cfg) {
        Scope<CoreSystems> cs = CreateScope<CoreSystems>();

        cs->platform = CreatePlatform(cfg);
        cs->platform->Initialize();
        cs->window   = CreateWindow(cfg);

        auto renderDevice = CreateRenderDevice(cfg);
        renderDevice->Initialize(cs->window->GetNativeHandle(), cfg.rendererConfig);

        cs->renderer = CreateRenderer(std::move(renderDevice), cfg);

        cs->physicsBackend = CreatePhysicsBackend(cfg);

        cs->sceneManager = CreateScope<SceneManager>();

        cs->systems = CreateScope<SystemManager>();

        cs->timer = CreateScope<RealTimeTimer>();

        cs->debugRenderer = CreateScope<graphics::debug::DebugRenderer>();
        Services::SetDebugRenderer(cs->debugRenderer.get());

        return std::move(cs);
    }

    Scope<AssetSystems> EngineBootstrap::BuildAssetSystems() {
        Scope<AssetSystems> as = CreateScope<AssetSystems>();

        // The order matters, because resourceLoader needs importer stuff
        as->assetManager   = CreateScope<AssetManager>();
        as->meshManager    = CreateScope<MeshManager>();
        as->assetImporter  = CreateScope<AssetImporter>();
        as->resourceLoader = CreateScope<ResourceLoader>();

        return as;
    }

    void EngineBootstrap::PopulateContext(IApplicationContext &ctx, CoreSystems& cs, AssetSystems& as, RegisteredSystems& rs) {
        ctx.SetWindow(cs.window.get());
        ctx.SetCameraSystem  (rs.cameraSystem);
        ctx.SetAssetManager  (as.assetManager.get());
        ctx.SetAssetImporter (as.assetImporter.get());
        ctx.SetMeshManager   (as.meshManager.get());
        ctx.SetResourceLoader(as.resourceLoader.get());
        ctx.SetSceneManager  (cs.sceneManager.get());
        ctx.SetDebugRenderer (cs.debugRenderer.get());
    }

    Scope<IWindow> EngineBootstrap::CreateWindow(const EngineConfig &cfg) {
        switch (cfg.window.backend)
        {
            case WindowBackend::GLFW:
                return CreateScope<platform::glfw::GLFWWindow>(cfg.window, cfg.renderer);
            case WindowBackend::SDL:
                throw std::runtime_error("SDL unsupported for now! Window can't initialized, change to GLFW");

            default:
                throw std::runtime_error("Unsupported Window library!");
        }
    }

    Scope<IPlatform> EngineBootstrap::CreatePlatform(const EngineConfig &cfg) {
        switch (cfg.window.backend) {
            case WindowBackend::GLFW: {
                return CreateScope<platform::GLFWPlatform>();
            }
            case WindowBackend::SDL:
                throw std::runtime_error("SDL unsupported for now! Platform can't initialized, change to GLFW");
                // return CreateScope<platform::GLFWPlatform>();

            default:
                throw std::runtime_error("Unsupported platform!");
        }
    }

    Scope<rhi::IRenderDevice> EngineBootstrap::CreateRenderDevice(const EngineConfig &cfg) {
        switch (cfg.graphicsAPI)
        {
            case rhi::GraphicsAPI::OpenGL:
                return CreateScope<platform::opengl::OpenGLRenderDevice>();

            case rhi::GraphicsAPI::Vulkan:
                return CreateScope<platform::Vk::VkRenderDevice>();

            default:
                throw std::runtime_error("Unsupported graphics API!");
        }
    }

    Scope<IPhysicsBackend> EngineBootstrap::CreatePhysicsBackend(const EngineConfig &cfg) {
        switch (cfg.physicsBackend)
        {
            case physics::PhysicsBackend::PhysX:
                return CreateScope<physics::PhysXBackend>();

            // case PhysicsBackendType::Bullet:
            //     return CreateScope<physics::BulletBackend>();

            default:
                throw std::runtime_error("Unsupported physics backend!");
        }
    }

    Scope<IRenderer> EngineBootstrap::CreateRenderer(Scope<rhi::IRenderDevice> graphicsBackend, const EngineConfig &cfg) {
        switch (cfg.graphicsAPI)
        {
            case graphics::GraphicsAPI::OpenGL:
                return CreateScope<platform::opengl::OpenGLRenderer>(std::move(graphicsBackend));

            case graphics::GraphicsAPI::Vulkan:
                return CreateScope<platform::Vk::VkRenderer>(std::move(graphicsBackend));

            default:
                throw std::runtime_error("Unsupported renderer type");
        }
    }

    EngineBootstrap::RegisteredSystems EngineBootstrap::RegisterSystems(SystemManager *sysMngr, IPhysicsBackend& physicsBackend) {
        RegisteredSystems rs;

        rs.cameraSystem = sysMngr->AddSystem<ecs::CameraSystem>();
        sysMngr->AddSystem<ecs::PhysicsSystem>(physicsBackend);
        sysMngr->AddSystem<ecs::MovementSystem>();
        sysMngr->AddSystem<ecs::MeshRendererSystem>();
        sysMngr->AddSystem<ecs::LightSystem>();

        return rs;
    }
}
