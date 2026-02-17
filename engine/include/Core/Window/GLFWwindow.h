//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "IWindow.h"
#include "Common/RealTypes.h"

struct GLFWwindow;
struct GLFWmonitor;

namespace Real::platform::glfw {

    class GLFWWindow final : public core::IWindow {
    public:
        void Init(const char *title, int width, int height) override;
        void PollEvents() override;
        void SwapBuffers() override;
        bool ShouldClose() const override;
        void Close() override;
        void Resize(int width, int height) override;
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        bool CheckOpenGLVersion();

    private:
        GLFWwindow* m_Window = nullptr;
        GLFWmonitor* m_GLFWMonitor = nullptr;
        GLFWwindow* m_GLFWShare = nullptr;

        int m_Width = 1280;
        int m_Height = 760;
        String m_Title = "Real";
    };
}
