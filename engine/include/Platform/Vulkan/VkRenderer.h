//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "RHI/IRenderer.h"
#include "Core/Utils.h"

namespace Real::rhi { class IRenderDevice; }

namespace Real::platform::Vk {

    class VkRenderer final : public rhi::IRenderer {
    public:
        explicit VkRenderer(Scope<rhi::IRenderDevice> device);

        void Init() override;
        void Render(Scene *scene, Entity *camera) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;

    private:
        Scope<rhi::IRenderDevice> m_Device;
    };
}
