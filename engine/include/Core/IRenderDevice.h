//
// Created by pointerlost on 2/19/26.
//
#pragma once

namespace Real::graphics {
    struct Color;
}

namespace Real::graphics {
    enum class ClearFlags : unsigned char;
}

namespace Real {
    struct RendererConfig;
}

namespace Real::core {

    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;

        virtual void Initialize(void* nativeWindow, const RendererConfig& cfg) = 0;
        virtual void Shutdown() = 0;

        virtual void SwapBuffers() = 0;
        virtual void ClearColor(const graphics::Color& /* color */) = 0;
        virtual void Clear(graphics::ClearFlags /* clearFlags */) = 0;
    };
}
