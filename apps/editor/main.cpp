//
// Created by pointerlost on 2/18/26.
//
#include <memory>

#include "Core/Engine/EngineBootstrap.h"
#include "Window/WindowConfig.h"
#include "EditorApplication.h"
#include "Core/Engine/EngineCore.h"
#include "include/ApplicationBuilder.h"
#include "Graphics/Shader.h"

int main() {

    try
    {
        Real::EngineConfig cfg;
        cfg.mode            = Real::EngineMode::Editor;
        cfg.window.width    = 1520;
        cfg.window.height   = 840;
        cfg.window.title    = "Real";
        cfg.window.backend  = Real::core::WindowBackend::GLFW;
        cfg.graphicsAPI     = Real::graphics::GraphicsAPI::OpenGL;
        cfg.physicsBackend  = Real::physics::PhysicsBackend::PhysX;

        auto app = Real::ApplicationBuilder::Build(Real::ApplicationMode::Editor);
        auto engineCore = Real::core::EngineBootstrap::Build(cfg, std::move(app));

        engineCore->Start();
        engineCore->RunLoop();
        engineCore->Stop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    /*
    const auto engine = Real::CreateScope<Real::Engine>();
    engine->InitResources();
    engine->InitGameResources(); // This is not permanent, I'll remove after adding game state
    engine->Running();
    */

    return 0;
}
