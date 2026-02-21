//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Core/IRenderDevice.h"

namespace Real::platform::vk {

    class VkRenderDevice final : public core::IRenderDevice {
    public:
        void Initialize(void *nativeWindow, const RendererConfig &cfg) override;
        void Shutdown() override;

        void SwapBuffers() override;
        void ClearColor(const graphics::Color& color) override;
        void Clear(graphics::ClearFlags clearFlags) override;
    };
}
