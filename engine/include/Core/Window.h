//
// Created by pointerlost on 10/3/25.
//
#pragma once
#include <glad/include/glad/glad.h>
#include <GLFW/glfw3.h>
#include "Common/RealTypes.h"

namespace Real::Graphics {

    class Window {
    public:
        Window(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

        void Init();

        void SetWidth(int width) { m_Width = width; }
        void SetHeight(int height) { m_Height = height; }

        [[nodiscard]] GLFWwindow* GetGLFWWindow() const { return m_Window; }

    private:
        int m_Width = 720;
        int m_Height = 480;
        String m_Title;

        GLFWwindow* m_Window = nullptr;
        GLFWmonitor* m_GlfwMonitor = nullptr;
        GLFWwindow* m_GlfwShare = nullptr;

    private:
        bool CheckOpenGLVersion();
    };
}
