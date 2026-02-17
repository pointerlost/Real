//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "RenderContext.h"
#include "Core/Utils.h"

namespace Real {
    class Scene;
    class Entity;
}

namespace Real::opengl {

    class OpenGLRenderer {
    public:
        explicit OpenGLRenderer(Scene* scene);
        void Render(Entity* camera);

        [[nodiscard]] RenderContext* GetRenderContext() const { return m_SceneRenderContext.get(); }

    private:
        Scene* m_Scene;
        Scope<RenderContext> m_SceneRenderContext;

    private:
        void BindGPUBuffers() const;
    };
}
