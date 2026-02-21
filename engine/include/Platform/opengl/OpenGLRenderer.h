//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/RealTypes.h"
#include "Core/IRenderDevice.h"
#include "Core/IRenderer.h"
#include "Core/Utils.h"

namespace Real {
    class RenderContext;
    class Scene;
}

namespace Real::platform::opengl {

    class OpenGLRenderer final : public core::IRenderer {
    public:
        explicit OpenGLRenderer(Scope<core::IRenderDevice> device);

        void Init() override;
        void Render(Scene* scene, Entity *camera) override;
        void Shutdown() override;
        void BeginFrame() override;
        void EndFrame() override;

    private:
        Scope<core::IRenderDevice> m_Device;
        RenderContext* m_RenderContext = nullptr;

        RendererConfig config;

    private:
        void BindGPUBuffers();
    };
}
