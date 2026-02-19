//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "IWindow.h"
#include "Common/RealTypes.h"

namespace Real::core {
    struct WindowConfig;
}

struct GLFWwindow;
struct GLFWmonitor;

namespace Real::platform::glfw {

    class GLFWWindow final : public core::IWindow {
    public:
        explicit GLFWWindow(const core::WindowConfig& cfg, const RendererConfig& renderConfig);
        void PollEvents() override;
        void SwapBuffers() override;
        [[nodiscard]] bool ShouldClose() const override;
        void Close() override;
        void Resize(int width, int height) override;
        [[nodiscard]] void* GetNativeHandle() const override;

    private:
        GLFWwindow*  m_Window      = nullptr;
        GLFWmonitor* m_GLFWMonitor = nullptr;
        GLFWwindow*  m_GLFWShare   = nullptr;

        int m_Width    = 1520;
        int m_Height   = 840;
        String m_Title = "Real";
        WindowType type = WindowType::glfw;
    };
}
