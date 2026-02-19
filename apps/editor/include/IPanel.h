//
// Created by pointerlost on 10/17/25.
//
#pragma once

namespace Real::opengl {
    class OpenGLRenderer;
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
        virtual void OnSceneAttached(Scene* scene) = 0;
        virtual void OnImGuiRender() = 0;
    };
}
