//
// Created by pointerlost on 2/16/26.
//
#include "Core/Window/GLFWwindow.h"
#include "Core/Logger.h"
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstring>

namespace Real::platform::glfw {

    void GLFWWindow::Init(const char *title, int width, int height) {
        m_Title = title;
        m_Width = width;
        m_Height = height;

        if (!glfwInit()) {
            Error("glfwInit failed!");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), m_GLFWMonitor, m_GLFWShare);
        if (!m_Window) {
            Error("glfwCreateWindow failed!");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGL()) {
            Error("Failed to load GLAD!");
            return;
        }

        if (!CheckOpenGLVersion()) {
            Warn("Opengl version fucked up!");
        }

        // Center the cursor when the program start
        glfwSetCursorPos(m_Window, static_cast<double>(width) / 2, static_cast<double>(height) / 2);
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

    bool GLFWWindow::CheckOpenGLVersion() {
        // Get version info
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);

        Info("OpenGL version: " + std::to_string(major) + "." + std::to_string(minor));
        Info("OpenGL vendor: " + String(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
        Info("OpenGL renderer: " + String(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));

        // Check if we have at least OpenGL 4.4
        if (major < 4 || (major == 4 && minor < 4)) {
            Warn("OpenGL 4.4 is not supported! Your version: "
                + std::to_string(major) + "." + std::to_string(minor));
            return false;
        }

        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);

        for (GLint i = 0; i < n; i++) {
            if (const auto ext = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i)); strcmp(ext, "GL_ARB_bindless_texture") == 0) {
                Info("Bindless textures supported!");
                return true;
            }
        }
        Warn("GL_ARB_bindless_texture is not supported!");
        return false;
    }
}
