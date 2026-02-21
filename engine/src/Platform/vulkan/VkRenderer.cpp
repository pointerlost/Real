//
// Created by pointerlost on 2/19/26.
//
#include "Platform/vulkan/VkRenderer.h"

namespace Real::platform::vk {

    VkRenderer::VkRenderer(Scope<core::IRenderDevice> device)
        :m_Device(std::move(device))
    {
    }

    void VkRenderer::Init() {
    }

    void VkRenderer::Render(Scene *scene, Entity *camera) {
    }

    void VkRenderer::Shutdown() {
    }

    void VkRenderer::BeginFrame() {
    }

    void VkRenderer::EndFrame() {
    }
}
