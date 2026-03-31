//
// Created by pointerlost on 10/13/25.
//
#pragma once
#include <unordered_map>
#include "GPUBuffers.h"
#include "Core/UUID.h"

namespace Real {
    namespace assets {
        struct MeshAsset;
    }

    struct TransformComponent;
    class Entity;
    class Scene;
}

namespace Real::graphics {

    struct RenderableData {
        const assets::MeshAsset* mesh = nullptr;
        UUID                     materialUUID{};
        // TODO: transform per mesh
    };

    class RenderContext {
    public:
        void InitResources();
        void CollectRenderables(Scene* scene);

        [[nodiscard]] FrameRenderData&       GetGPURenderData()       { return m_FrameRenderData; }
        [[nodiscard]] const FrameRenderData& GetGPURenderData() const { return m_FrameRenderData; }

    private:
        FrameRenderData m_FrameRenderData;
        std::unordered_map<UUID, int> m_MaterialIdxCache; // TODO: This shouldn't be here (remove it)

    private:
        void CollectLight(const Entity* entity);
        void CollectCamera(const Entity* entity);
        int  BuildTransform(const TransformComponent& tc);
        int  BuildMaterial(const UUID& materialUUID);
        void PushDrawCommand(const assets::MeshAsset* mesh, int transformIndex, int materialIndex, uint baseInstance);
        void CollectGlobalData();
        void CleanPrevFrame();
        Vector<RenderableData> CollectRenderable(const Entity* entity);
    };
}
