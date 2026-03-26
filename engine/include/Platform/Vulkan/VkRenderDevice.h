//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "RHI/IRenderDevice.h"

namespace Real::platform::vk {

    class VkRenderDevice final : public rhi::IRenderDevice {
    public:
        void Initialize(void *nativeWindow, const RendererConfig &cfg) override;
        void Shutdown() override;
    };
}
