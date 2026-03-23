//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/Utils.h"

namespace Real {
    namespace rhi {
        class IRenderer;
        class IRenderDevice;
    }

    struct EngineConfig;

    namespace ecs {
        class CameraSystem;
    }

    namespace core {
        class IApplicationContext;
        class IPlatform;
        class IWindow;
        struct AssetSystems;
        struct CoreSystems;
        class SystemManager;
        class IPhysicsBackend;
        class IApplication;
        class EngineCore;
    }
}

namespace Real::core {

    class EngineBootstrap {
    public:
        static Scope<EngineCore> Build(const EngineConfig& cfg, Scope<IApplication> app);

    private:
        struct RegisteredSystems {
            ecs::CameraSystem* cameraSystem = nullptr;
        };

    private:
        static Scope<CoreSystems>  BuildCoreSystems(const EngineConfig& cfg);
        static Scope<AssetSystems> BuildAssetSystems();

        static RegisteredSystems RegisterSystems(SystemManager* sysMngr, IPhysicsBackend& physicsBackend);
        static void PopulateContext(IApplicationContext& ctx, CoreSystems& cs, AssetSystems& as, RegisteredSystems& rs);

        static Scope<IWindow> CreateWindow(const EngineConfig& cfg);
        static Scope<IPlatform> CreatePlatform(const EngineConfig& cfg);
        static Scope<rhi::IRenderDevice> CreateRenderDevice(const EngineConfig& cfg);
        static Scope<IPhysicsBackend> CreatePhysicsBackend(const EngineConfig& cfg);
        static Scope<rhi::IRenderer> CreateRenderer(Scope<rhi::IRenderDevice> graphicsBackend ,const EngineConfig& cfg);
    };
}
