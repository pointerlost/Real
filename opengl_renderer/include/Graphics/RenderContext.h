//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include "GPUBuffers.h"
#include "RenderCommand.h"
#include <vector>

#include "Buffer.h"
#include "../Core/Config.h"
#include "Core/Utils.h"

namespace Real {
    struct Texture;
}

namespace Real {
    class Scene;
}

namespace Real {

    struct GPUData {
        std::vector<TransformSSBO> transforms;
        std::vector<MaterialSSBO> materials;
        std::vector<LightSSBO> lights;
        std::vector<DrawElementsIndirectCommand> drawCommands;
        std::vector<EntityMetadata> entityData;
        CameraUBO camera;
        GlobalUBO globalData;
    };

    struct GPUBuffers {
        opengl::Buffer transform;
        opengl::Buffer material;
        opengl::Buffer light;
        opengl::Buffer drawCommand;
        opengl::Buffer entityData;
        opengl::Buffer camera;
        opengl::Buffer globalData;
    };

    class RenderContext {
    public:
        explicit RenderContext(Scene* scene);
        void InitResources();
        void BindGPUBuffers() const;
        void CollectRenderables();

        [[nodiscard]] const GPUData& GetGPURenderData() const { return m_GPUDatas; }
        [[nodiscard]] const GPUBuffers& GetBuffers() const { return m_Buffers; }

    private:
        GPUData m_GPUDatas{};
        GPUBuffers m_Buffers{};
        Scene* m_Scene;

    private:
        void CollectCamera();
        void CollectLights();
        void CollectGlobalData();
        void CleanPrevFrame();
        void UploadToGPU();
    };
}
