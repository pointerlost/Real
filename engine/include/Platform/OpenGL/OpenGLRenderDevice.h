//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include "RHI/IRenderDevice.h"

namespace Real { struct OpenGLConfig; }

namespace Real::platform::opengl {

    class OpenGLRenderDevice final : public rhi::IRenderDevice {
    public:
        void Initialize(void *nativeWindow, const RendererConfig &cfg) override;
        void Shutdown()                                                override;

    private:
        void CheckGLVersion(const OpenGLConfig& cfg);
    };

}
