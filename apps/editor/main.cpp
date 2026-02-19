//
// Created by pointerlost on 2/18/26.
//
#include <memory>

#include "Core/Engine/EngineBootstrap.h"
#include "Core/Engine/EngineCore.h"
#include "Core/Window/GLFWPlatform.h"
#include "Core/Window/GLFWwindow.h"
#include "Core/Window/WindowConfig.h"
#include "Editor/Editor.h"
#include "Physics/Physx/PhysXBackend.h"
#include "Platform/opengl/OpenGLAPI.h"
#include "Scene/Systems/CameraSystem.h"
#include "Scene/Systems/LightSystem.h"
#include "Scene/Systems/MeshRendererSystem.h"
#include "Scene/Systems/MovementSystem.h"
#include "Scene/Systems/PhysicsSystem.h"
#include "Platform/opengl/OpenGLRenderer.h"
#include "Scene/Scene.h"
#include "Scene/Systems/SystemsManager.h"
#include "Timer/Timer.h"

int main() {

    try
    {
        Real::core::EngineConfig cfg;
        cfg.editorMode = true;
        cfg.windowConfig.width  = 1520;
        cfg.windowConfig.height = 840;
        cfg.windowConfig.title  = "Real";
        cfg.apiType = Real::API::OpenGL;
        cfg.physicsBackendType = Real::PhysicsBackendType::PhysX;

        Real::core::EngineCore engine(cfg);
        engine.Start();
        engine.RunLoop();
        engine.Stop();
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
