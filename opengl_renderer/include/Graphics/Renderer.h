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

    class Renderer {
    public:
        explicit Renderer(Scene* scene);
        void Render(Entity* camera);

        [[nodiscard]] RenderContext* GetRenderContext() const { return sceneRenderContext.get(); }

    private:
        Scene* m_Scene;
        Scope<RenderContext> sceneRenderContext;

    private:
        void BindGPUBuffers() const;
    };
}
