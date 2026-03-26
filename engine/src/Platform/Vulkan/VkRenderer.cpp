//
// Created by pointerlost on 2/19/26.
//
#include "Platform/Vulkan/VkRenderer.h"
#include "RHI/IRenderDevice.h"

namespace Real::platform::vk {

    VkRenderer::VkRenderer(Scope<rhi::IRenderDevice> device)
        : m_Device(std::move(device)), m_RenderContext(CreateScope<graphics::RenderContext>())
    {
    }

    void VkRenderer::Init() {
    }

    void VkRenderer::Render(Scene *scene, Entity *camera) {
    }

    void VkRenderer::Shutdown() {
    }

    void VkRenderer::BeginFrame(const graphics::FrameConfig& fc) {
    }

    void VkRenderer::EndFrame() {
    }

    void VkRenderer::SwapBuffers() {
    }

    void VkRenderer::ClearColor(const graphics::Color &color) {
    }

    void VkRenderer::Clear(const graphics::ClearFlags &clearFlags) {
    }
}
