//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Core/IGraphicsBackend.h"

namespace Real::platform::vk {

    class VkBackend final : public core::IGraphicsBackend {
        void Initialize(void *nativeWindow, const RendererConfig &cfg) override;
        void Shutdown() override;
    };
}
