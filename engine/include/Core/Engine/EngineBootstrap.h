//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/RealTypes.h"
#include "Core/Utils.h"
#include "Core/Window/WindowConfig.h"


namespace Real::physics {
    class IPhysicsBackend;
}

namespace Real::core {
    class IGraphicsBackend;
}

namespace Real::core {
    class SystemManager;
}

namespace Real::core {
    class IRenderer;
    class IWindow;
    class EngineCore;
}

namespace Real::core {

    struct EngineConfig {
        bool editorMode = true;
        WindowConfig windowConfig;
        RendererConfig rendererConfig;
        API apiType;
        RendererType rendererType;
        PhysicsBackendType physicsBackendType;
    };

    class EngineBootstrap {
    public:
        static Scope<EngineCore> Build(const EngineConfig& cfg);

    private:
        static Scope<IWindow> CreateWindow(const EngineConfig& cfg);
        static Scope<IGraphicsBackend> CreateGraphicsBackend(const EngineConfig& cfg, IWindow& window);
        static Scope<physics::IPhysicsBackend> CreatePhysicsBackend(const EngineConfig& cfg);
        static Scope<IRenderer> CreateRenderer(const EngineConfig& cfg, IWindow& window);
        static void RegisterSystems(SystemManager* sysMngr, Scope<physics::IPhysicsBackend> physicsBackend);
    };
}