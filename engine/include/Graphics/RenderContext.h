//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <unordered_map>
#include "GPUBuffers.h"
#include "Core/UUID.h"

namespace Real {
    struct MeshAsset;
    struct TransformComponent;
    class Entity;
    class Scene;
}

namespace Real::graphics {

    struct RenderableData {
        const MeshAsset* mesh = nullptr;
        UUID             materialUUID{};
        // TODO: transform per mesh
    };

    class RenderContext {
    public:
        void InitResources();
        void CollectRenderables(Scene* scene);

        FrameRenderData& GetGPURenderData() { return m_FrameRenderData; }
        [[nodiscard]] const FrameRenderData& GetGPURenderData() const { return m_FrameRenderData; }

    private:
        FrameRenderData m_FrameRenderData;
        std::unordered_map<UUID, int> m_MaterialIdxCache; // TODO: This shouldn't be here (remove it)

    private:
        void CollectLight(const Entity* entity);
        void CollectCamera(const Entity* entity);
        int BuildTransform(const TransformComponent& tc);
        int BuildMaterial(const UUID& materialUUID);
        void PushDrawCommand(const MeshAsset* mesh, int transformIndex, int materialIndex, uint baseInstance);
        Vector<RenderableData> CollectRenderables(const Entity* entity);
        void CollectGlobalData();
        void CleanPrevFrame();
    };
}
