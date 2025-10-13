//
// Created by pointerlost on 10/3/25.
//
#include "Core/Window.h"
#include "Core/Logger.h"
#include "Graphics/Config.h"

namespace Real::Graphics {

    Window::Window(int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
        :m_Width(width), m_Height(height), m_Title(title), m_GlfwMonitor(monitor), m_GlfwShare(share)
    {
    }

    void Window::Init() {
        if (!glfwInit()) {
            Error("glfwInit failed!");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), m_GlfwMonitor, m_GlfwShare);
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

        // Center the cursor when the program start
        glfwSetCursorPos(m_Window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    }
}
