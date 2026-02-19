//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Core/IRenderer.h"

namespace Real {
    class RenderContext;
}

namespace Real {
    class Scene;
}

namespace Real::platform::opengl {

    class OpenGLRenderer final : public core::IRenderer {
    public:
        void Init() override;
        void Render(Scene* scene, Entity *camera) override;
        void Shutdown() override;

    private:
        RenderContext* m_RenderContext = nullptr;

    private:
        void BindGPUBuffers();
    };
}
