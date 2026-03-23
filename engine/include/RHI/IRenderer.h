//
// Created by pointerlost on 2/16/26.
//
#pragma once

namespace Real {
    class Entity;
    class Scene;
}

namespace Real::rhi {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;
        virtual void Init() = 0;
        virtual void Render(Scene* scene, Entity* camera) = 0;
        virtual void Shutdown() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual IRenderContext& GetContext() = 0;
    };
}