//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/Utils.h"

namespace Real {
    struct EngineConfig;

    namespace rhi {
        class IRenderer;
        class IRenderDevice;
    }

    namespace ecs {
        class CameraSystem;
    }
    namespace core {
        class IPlatform;
        class IWindow;
        class IApplicationContext;
        class SystemManager;
        class IPhysicsBackend;
        struct AssetSystems;
        struct CoreSystems;
        class IApplication;
        class EngineCore;
    }

}

namespace Real::core {

    class EngineBootstrap {
    public:
        static Scope<EngineCore> Build(const EngineConfig& cfg, Scope<IApplication> app);

    private:
        struct RegisteredSystems { ecs::CameraSystem* cameraSystem = nullptr; };

    private:
        static Scope<CoreSystems>  BuildCoreSystems(const EngineConfig& cfg);
        static Scope<AssetSystems> BuildAssetSystems();
        static RegisteredSystems   RegisterSystems(SystemManager* sysMngr, IPhysicsBackend& physicsBackend);

        static void PopulateContext(
            IApplicationContext& ctx,
            const CoreSystems& cs,
            AssetSystems& as,
            const RegisteredSystems& rs
        );

        static Scope<IWindow>            CreateWindow(const EngineConfig& cfg);
        static Scope<IPlatform>          CreatePlatform(const EngineConfig& cfg);
        static Scope<rhi::IRenderDevice> CreateRenderDevice(const EngineConfig& cfg);
        static Scope<IPhysicsBackend>    CreatePhysicsBackend(const EngineConfig& cfg);
        static Scope<rhi::IRenderer>     CreateRenderer(
            Scope<rhi::IRenderDevice> graphicsBackend,
            const EngineConfig& cfg
        );
    };
}
