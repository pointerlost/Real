//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Core/IRenderDevice.h"
#include "Core/IRenderer.h"
#include "Core/Utils.h"

namespace Real::platform::vk {

    class VkRenderer final : public core::IRenderer {
    public:
        explicit VkRenderer(Scope<core::IRenderDevice> device);

        void Init() override;
        void Render(Scene *scene, Entity *camera) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;

    private:
        Scope<core::IRenderDevice> m_Device;
    };
}
