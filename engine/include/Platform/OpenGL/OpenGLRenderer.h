//
// Created by pointerlost on 2/16/26.
//
#pragma once
#include "Common/Utils.h"
#include "Core/RealConfig.h"
#include "Graphics/RenderContext.h"
#include "RHI/IRenderDevice.h"
#include "RHI/IRenderer.h"
#include "Graphics/GPUBuffers.h"

namespace Real::platform::opengl {
    using namespace graphics;

    class OpenGLRenderer final : public rhi::IRenderer {
    public:
        explicit OpenGLRenderer(Scope<rhi::IRenderDevice> device);

        void Init() override;
        void Render(Scene* scene, Entity *camera) override;
        void Shutdown() override;
        void BeginFrame(const FrameConfig& fc) override;
        void EndFrame() override;

        RenderContext& GetRenderContext() override { return *m_RenderContext.get(); }

    private:
        Scope<rhi::IRenderDevice> m_Device;
        OpenGLBuffers m_Buffers;

        Scope<RenderContext> m_RenderContext = nullptr;

        RendererConfig config;

    private:
        void InitResources();

        void BindGPUBuffers() const;

        void UploadToGPU();

        void SwapBuffers();
        void ClearColor(const Color& color);
        void Clear(const ClearFlags& clearFlags);
    };
}
