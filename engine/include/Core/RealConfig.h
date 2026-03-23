//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "IPhysicsBackend.h"
#include "RHI/RHITypes.h"
#include "Window/WindowConfig.h"

constexpr float SCREEN_WIDTH  = 1520.0f;
constexpr float SCREEN_HEIGHT = 840.0f;

namespace Real {

    struct WindowConfig {
        u32    width          = 1520;
        u32    height         = 840;
        String title          = "Real";
        core::WindowBackend backend = core::WindowBackend::GLFW;
    };

    struct OpenGLConfig {
        int  major        = 4;
        int  minor        = 5;
        bool coreProfile  = true;
        bool debugContext = false;
        bool depthTesting = true;
        bool vsync        = true;
    };

    struct VulkanConfig {
    };

    struct RendererConfig {
        OpenGLConfig opengl;
        VulkanConfig vulkan;
    };

    enum class EngineMode {
        Editor,
        Runtime
    };

    struct EngineConfig {
        EngineMode mode                        = EngineMode::Editor;
        rhi::GraphicsAPI   graphicsAPI         = rhi::GraphicsAPI::OpenGL;
        physics::PhysicsBackend physicsBackend = physics::PhysicsBackend::PhysX;

        WindowConfig   window;
        RendererConfig renderer;
    };
}