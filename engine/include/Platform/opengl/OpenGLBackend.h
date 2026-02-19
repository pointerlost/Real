//
// Created by pointerlost on 2/17/26.
//
#pragma once
#include "Core/IGraphicsBackend.h"

namespace Real {
    struct OpenGLConfig;
}

namespace Real::platform::opengl {

    class OpenGLBackend final : public core::IGraphicsBackend {
    public:
        void Initialize(void *nativeWindow, const RendererConfig &cfg) override;
        void Shutdown() override;

    private:
        void CheckOpenGLVersion(const OpenGLConfig& cfg);
    };

}
