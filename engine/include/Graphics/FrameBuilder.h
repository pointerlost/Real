//
// Created by pointerlost on 2/25/26.
//
#pragma once

namespace Real {
    class RenderContext;
    class Scene;
}

namespace Real::graphics {

    class FrameBuilder {
    public:
        void Build(Scene* scene, RenderContext& context);
    };
}
