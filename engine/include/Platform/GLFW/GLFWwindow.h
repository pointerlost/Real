//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/Types.h"
#include "Window/IWindow.h"
#include "Window/WindowConfig.h"

namespace Real {
    struct RendererConfig;
}

namespace Real {
    namespace rhi { enum class GraphicsAPI; }
}
struct GLFWwindow;
struct GLFWmonitor;

namespace Real::platform::glfw {

    class GLFWWindow final : public core::IWindow {
    public:
        GLFWWindow(const core::WindowConfig& cfg, const RendererConfig& rc);

        void               PollEvents()                  override;
        void               SwapBuffers()                 override;
        [[nodiscard]] bool ShouldClose()           const override;
        void               Close()                       override;
        void               Resize(int width, int height) override;
        [[nodiscard]] void* GetNativeHandle()      const override;

    private:
        GLFWwindow*  m_Window      = nullptr;
        GLFWmonitor* m_GLFWMonitor = nullptr;
        GLFWwindow*  m_GLFWShare   = nullptr;

        int                m_Width  = 1520;
        int                m_Height = 840;
        std::string        m_Title  = "Real";
        core::WindowConfig m_Config = {};

    private:
        static void set_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void set_mouse_callback(GLFWwindow *window, f64 xpos, f64 ypos);
        static void set_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void set_mouse_scroll_callback(GLFWwindow *window, f64 xoffset, f64 yoffset);
        static void set_framebuffer_size_callback(GLFWwindow *window, int width, int height);

        void InitCallbacks(GLFWwindow *window);

        bool m_FirstMouse = true;
    };
}
