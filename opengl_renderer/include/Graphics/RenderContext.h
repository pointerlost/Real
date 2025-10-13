//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include "Config.h"
#include "GPUBuffers.h"
#include "RenderCommand.h"
#include <vector>

namespace Real {
    class Scene;
}

namespace Real {

    struct GPUBuffers {
        std::vector<TransformSSBO> transforms;
        std::vector<MaterialSSBO> materials;
        std::vector<LightSSBO> lights;
        std::vector<DrawElementsIndirectCommand> commands;
        std::vector<EntityMetadata> entityData;
        CameraUBO camera;
    };

    class RenderContext {
    public:
        explicit RenderContext(Scene* scene);
        void InitResources();
        void CollectRenderables();

    private:
        GPUBuffers m_GPUBuffers;
        Scene* m_Scene;

    private:
        void CollectCamera();
        void CollectLights();
    };
}
