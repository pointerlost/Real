//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/RealTypes.h"
#include "Core/Utils.h"
#include "Core/Window/WindowConfig.h"

namespace Real {
    namespace physics {
        class IPhysicsBackend;
    }
    namespace core {
        class IPlatform;
        class IRenderDevice;
        class SystemManager;
        class IRenderer;
        class IWindow;
        class EngineCore;
    }
}

namespace Real::core {

    class EngineBootstrap {
    public:
        static Scope<EngineCore> Build(const EngineConfig& cfg);

    private:
        static CoreSystems   BuildCoreSystems(const EngineConfig& cfg);
        static AssetSystems  BuildAssetSystems();

        static Scope<IWindow> CreateWindow(const EngineConfig& cfg);
        static Scope<IPlatform> CreatePlatform(const EngineConfig& cfg);
        static Scope<IRenderDevice> CreateRenderDevice(const EngineConfig& cfg, IWindow& window);
        static Scope<physics::IPhysicsBackend> CreatePhysicsBackend(const EngineConfig& cfg);
        static Scope<IRenderer> CreateRenderer(Scope<IRenderDevice> graphicsBackend ,const EngineConfig& cfg);
        static void RegisterSystems(SystemManager* sysMngr, physics::IPhysicsBackend& physicsBackend);
    };
}