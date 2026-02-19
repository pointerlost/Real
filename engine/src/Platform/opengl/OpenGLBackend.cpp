//
// Created by pointerlost on 2/17/26.
//
#include <cstring>
#include <Platform/opengl/OpenGLBackend.h>
#include "glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "Common/RealTypes.h"
#include "Core/Logger.h"

namespace Real::platform::opengl {

    void OpenGLBackend::Initialize(void *nativeWindow, const RendererConfig& cfg)
    {
        const auto window = static_cast<GLFWwindow*>(nativeWindow);

        glfwMakeContextCurrent(window);

        if (!gladLoadGL())
            throw std::runtime_error("Failed to load GL");

        glfwSwapInterval(cfg.opengl.vsync ? 1 : 0);

        if (cfg.opengl.debugContext) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        }

        CheckOpenGLVersion(cfg.opengl);
    }

    void OpenGLBackend::Shutdown() {
    }

    void OpenGLBackend::CheckOpenGLVersion(const OpenGLConfig& cfg) {
        // Get version info
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        Info("OpenGL version: "  + std::to_string(major) + "." + std::to_string(minor));
        Info("OpenGL vendor: "   + String(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
        Info("OpenGL renderer: " + String(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

        // Check if we have at least OpenGL 4.4
        if (major < cfg.major || (major == cfg.major && minor < cfg.minor))
        {
            Warn("Requested OpenGL version not supported!");
        }

        bool bindlessSupported = false;

        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);

        for (GLint i = 0; i < n; i++) {
            auto ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));

            if (ext && strcmp(ext, "GL_ARB_bindless_texture") == 0) {
                bindlessSupported = true;
                break;
            }
        }

        if (bindlessSupported)
            Info("Bindless textures supported!");
        else
            Warn("GL_ARB_bindless_texture is not supported!");
    }
}
