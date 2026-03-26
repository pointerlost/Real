//
// Created by pointerlost on 2/16/26.
//
#pragma once

namespace Real::graphics {
    struct FrameConfig;
}

namespace Real {
    namespace graphics {
        class RenderContext;
    }

    class Entity;
    class Scene;
}

namespace Real::rhi {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual void Init()                               = 0;
        virtual void Render(Scene* scene, Entity* camera) = 0;
        virtual void Shutdown()                           = 0;

        virtual void BeginFrame(const graphics::FrameConfig& /*FrameConfig*/) = 0;
        virtual void EndFrame()                                               = 0;

        virtual graphics::RenderContext& GetRenderContext() = 0;
    };
}