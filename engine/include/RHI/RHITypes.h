//
// Created by pointerlost on 3/22/26.
//
#pragma once

namespace Real::rhi {

    enum class GraphicsAPI {
        OpenGL,
        Vulkan,
        DX12,   // easy to add later
        Metal,
    };

    constexpr auto ACTIVE_RENDER_BACKEND = GraphicsAPI::OpenGL;

}