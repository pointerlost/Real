//
// Created by pointerlost on 10/17/25.
//
#pragma once

namespace Real::opengl {
    class Renderer;
}

namespace Real {
    class Scene;
}

namespace Real::Graphics {
    class Window;
}

namespace Real::UI {

    class IPanel {
    public:
        virtual ~IPanel() = default;
        virtual void BeginFrame() = 0;
        virtual void Render(Scene* scene, opengl::Renderer* renderer) = 0;
        virtual void EndFrame() = 0;
        virtual void Shutdown() = 0;
    };
}
