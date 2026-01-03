//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <unordered_map>

#include "GPUBuffers.h"
#include "RenderCommand.h"
#include <vector>
#include "Buffer.h"
#include "Core/UUID.h"

namespace Real {
    struct RenderableData;
    struct MeshAsset;
    struct TransformComponent;
    class Entity;
    class Scene;
}

namespace Real {

    struct GPUData {
        std::vector<TransformSSBO> transforms;
        std::vector<MaterialSSBO> materials;
        std::vector<GLuint64> textures;
        std::vector<LightSSBO> lights;
        std::vector<DrawElementsIndirectCommand> drawCommands;
        std::vector<EntityMetadata> entityData;
        CameraUBO camera;
        GlobalUBO globalData;
    };

    struct GPUBuffers {
        opengl::Buffer transform;
        opengl::Buffer material;
        opengl::Buffer texture;
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

        GPUData& GetGPURenderData() { return m_GPUDatas; }
        [[nodiscard]] const GPUData& GetGPURenderData() const { return m_GPUDatas; }
        [[nodiscard]] const GPUBuffers& GetBuffers() const { return m_Buffers; }

    private:
        GPUData m_GPUDatas{};
        GPUBuffers m_Buffers{};
        Scene* m_Scene;
        std::unordered_map<UUID, int> m_MaterialIdxCache;

    private:
        void CollectLight(const Entity* entity);
        void CollectCamera(const Entity* entity);
        int PushTransform(TransformComponent& tc);
        int PushMaterial(const UUID& materialUUID);
        void PushDrawCommand(const MeshAsset* mesh, int transformIndex, int materialIndex, uint baseInstance);
        std::vector<RenderableData> CollectRenderables(const Entity* entity);
        void CollectGlobalData();
        void CleanPrevFrame();
        void UploadToGPU();
    };
}
