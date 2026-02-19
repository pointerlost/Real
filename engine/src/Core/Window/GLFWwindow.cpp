//
// Created by pointerlost on 2/16/26.
//
#include "Core/Window/GLFWwindow.h"
#include "Core/Logger.h"
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstring>

#include "Core/Window/WindowConfig.h"

namespace Real::platform::glfw {

    GLFWWindow::GLFWWindow(const core::WindowConfig &cfg, const RendererConfig& renderConfig)
        : m_Width(cfg.width), m_Height(cfg.height), m_Title(cfg.title), type(cfg.type)
    {
        if (renderConfig.type == RendererType::OpenGL)
        {
            const auto& glCfg = renderConfig.opengl;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glCfg.major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glCfg.minor);

            if (glCfg.coreProfile)
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            if (glCfg.debugContext)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        }

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

        if (!m_Window)
            throw std::runtime_error("glfwCreateWindow failed");

        // Center the cursor when the program start
        glfwSetCursorPos(m_Window, static_cast<double>(m_Width) / 2, static_cast<double>(m_Height) / 2);
    }

    void GLFWWindow::PollEvents() {
        glfwPollEvents();
    }

    void GLFWWindow::SwapBuffers() {
        glfwSwapBuffers(m_Window);
    }

    bool GLFWWindow::ShouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    void GLFWWindow::Close() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void GLFWWindow::Resize(int width, int height) {
        m_Width = width;
        m_Height = height;
    }

    void* GLFWWindow::GetNativeHandle() const {
        return m_Window;
    }
}
