//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "RHI/IRenderer.h"
#include "Graphics/RenderContext.h"
#include "Common/Utils.h"

namespace Real::graphics {
    enum class ClearFlags : uint8_t;
    struct Color;
}

namespace Real::rhi { class IRenderDevice; }

namespace Real::platform::vk {

    class VkRenderer final : public rhi::IRenderer {
    public:
        explicit VkRenderer(Scope<rhi::IRenderDevice> device);

        void Init() override;
        void Render(Scene *scene, Entity *camera) override;
        void Shutdown() override;

        void BeginFrame(const graphics::FrameConfig& fc) override;
        void EndFrame() override;

        graphics::RenderContext &GetRenderContext() override { return *m_RenderContext.get(); }

    private:
        Scope<rhi::IRenderDevice> m_Device;

        Scope<graphics::RenderContext> m_RenderContext;

    private:
        void SwapBuffers();
        void ClearColor(const graphics::Color& color);
        void Clear(const graphics::ClearFlags& clearFlags);
    };
}
