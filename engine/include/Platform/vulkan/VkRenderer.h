//
// Created by pointerlost on 2/19/26.
//
#pragma once
#include "Core/IRenderer.h"

namespace Real::platform::vk {

    class VkRenderer final : public core::IRenderer {
    public:
        void Init() override;
        void Render(Scene *scene, Entity *camera) override;
        void Shutdown() override;
    };
}
