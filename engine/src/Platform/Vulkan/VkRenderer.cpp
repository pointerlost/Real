//
// Created by pointerlost on 2/19/26.
//
#include "Platform/Vulkan/VkRenderer.h"
#include "RHI/IRenderDevice.h"

namespace Real::platform::Vk {

    VkRenderer::VkRenderer(Scope<rhi::IRenderDevice> device)
        : m_Device(std::move(device))
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
