//
// Created by pointerlost on 2/16/26.
//
#include "Platform/GLFW/GLFWwindow.h"
#include <stdexcept>
#include <GLFW/glfw3.h>
#include "Core/RealConfig.h"
#include "Input/Input.h"
#include "Input/Keycodes.h"
#include "RHI/RHITypes.h"

namespace Real::platform::glfw {

    GLFWWindow::GLFWWindow(const core::WindowConfig &cfg, const RendererConfig& rc)
        : m_Width(cfg.width), m_Height(cfg.height), m_Title(cfg.title), m_Config(cfg)
    {
        if constexpr (rhi::ACTIVE_RENDER_BACKEND == rhi::GraphicsAPI::OpenGL)
        {
            const auto& glCfg = rc.opengl;

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glCfg.major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glCfg.minor);

            if (glCfg.coreProfile)
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            if (glCfg.debugContext)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        }
        else if constexpr (rhi::ACTIVE_RENDER_BACKEND == rhi::GraphicsAPI::Vulkan)
        {
            // Do something
        }

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

        if (!m_Window)
            throw std::runtime_error("glfwCreateWindow failed");

        glfwSetWindowUserPointer(m_Window, this);

        // Center the cursor when the program start
        glfwSetCursorPos(m_Window, static_cast<double>(m_Width) / 2, static_cast<double>(m_Height) / 2);

        InitCallbacks(m_Window);
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

    void GLFWWindow::set_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            Input::KeyDown(true, key);
        }
        if (action == GLFW_RELEASE) {
            Input::KeyDown(false, key);
        }
    }

    void GLFWWindow::set_mouse_callback(GLFWwindow *window, f64 xpos, f64 ypos) {
        auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
        if (!self) return;

        if (self->m_FirstMouse) {
            Input::g_MouseLastX = xpos;
            Input::g_MouseLastY = ypos;
            self->m_FirstMouse = false;
        }

        Input::g_MouseLastX = Input::g_MouseX;
        Input::g_MouseLastY = Input::g_MouseY;
        Input::g_MouseX = xpos;
        Input::g_MouseY = ypos;

        // if (!Real::Services::GetEditorState()->FpsMode) return;
        if (Input::IsKeyHeld(REAL_MOUSE_BUTTON_LEFT) || Input::IsKeyPressed(REAL_MOUSE_BUTTON_LEFT)) {
            // Use inverted yaw because of "REAL" engine using right-handed coordinate system
            auto deltaX = Input::g_MouseLastX - Input::g_MouseX; // X-axis inverted
            auto deltaY = Input::g_MouseY - Input::g_MouseLastY;

            deltaX *= Input::g_Sensitivity;
            deltaY *= Input::g_Sensitivity;

            Input::g_Yaw   += static_cast<f32>(deltaX);
            Input::g_Pitch += static_cast<f32>(deltaY);

            if (Input::g_Pitch > 89.0f)  Input::g_Pitch = 89.0f;
            if (Input::g_Pitch < -89.0f) Input::g_Pitch = -89.0f;
        }
    }

    void GLFWWindow::set_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        if (button == REAL_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            Input::KeyDown(true, button);
        }
        if (action == GLFW_RELEASE) {
            Input::KeyDown(false, button);
        }
    }

    void GLFWWindow::set_mouse_scroll_callback(GLFWwindow *window, f64 xoffset, f64 yoffset) {
        Input::g_IsScrolling = true;
        Input::g_Scroll -= yoffset;
    }

    void GLFWWindow::set_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void GLFWWindow::InitCallbacks(GLFWwindow* window) {
        glfwSetKeyCallback(window, set_key_callback);
        glfwSetFramebufferSizeCallback(window, set_framebuffer_size_callback);
        glfwSetCursorPosCallback(window, set_mouse_callback);
        glfwSetMouseButtonCallback(window, set_mouse_button_callback);
        glfwSetScrollCallback(window, set_mouse_scroll_callback);
        // Set GLFW_CURSOR_DISABLED for first-person shooter camera
        // in normal case = GLFW_CURSOR_NORMAL
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
