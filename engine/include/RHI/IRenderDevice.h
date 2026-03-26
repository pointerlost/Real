//
// Created by pointerlost on 2/19/26.
//
#pragma once

namespace Real {
    struct RendererConfig;

    namespace graphics {
        struct Color;
        enum class ClearFlags : unsigned char;
    }
}

namespace Real::rhi {

    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;

        virtual void Initialize(void* nativeWindow, const RendererConfig& cfg) = 0;
        virtual void Shutdown()                                                = 0;
    };
}
