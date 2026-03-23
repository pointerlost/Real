//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/RenderContext.h"
#include "../../include/Assets/AssetManager.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"
#include "Util/Util.h"
#include "Core/Logger.h"
#include "Graphics/RenderCommand.h"
#include "Math/Conversions/GLMConversions.h"
#include "Scene/Entity.h"

namespace Real::graphics {

    void RenderContext::InitResources() {
    }

    void RenderContext::CollectRenderables(Scene* scene) {
        CleanPrevFrame();

        const auto view = scene->GetAllEntitiesWith<TransformComponent, IDComponent>();
        uint baseInstance = 0;

        for (auto [entity, transform, id] : view.each()) {
            const auto e = scene->GetEntityWithUUID(id.m_UUID);
            if (!e) continue;

            const int transformIndex = BuildTransform(transform);
            CollectCamera(e);
            CollectLight(e);

            for (const auto& [meshData, matUUID] : CollectRenderables(e)) {
                const int materialIndex = matUUID != 0 ? BuildMaterial(matUUID) : 0;
                PushDrawCommand(meshData, transformIndex, materialIndex, baseInstance);
                ++baseInstance;
            }
        }

        // Collect others
        CollectGlobalData();
    }

    void RenderContext::CollectCamera(const Entity* entity) {
        /*
         * TODO: An update is required to add multiple cameras during run-time
         * (currently, adding multiple cameras may cause to crash)!!
         */
        // const auto cam = Services::GetEditorState()->camera; // TODO: if it doesn't work REMEMBER THIS SHIT!
        if (entity->HasComponent<CameraComponent>()) {
            auto& cc = entity->GetComponentUnchecked<CameraComponent>();
            auto& tc = entity->GetComponentUnchecked<TransformComponent>();
            cc.m_Camera.ConvertToGPUFormat(tc.transform, m_FrameRenderData.camera);
        }
    }

    int RenderContext::BuildTransform(const TransformComponent& tc) {
        TransformSSBO gpu{};
        const int index = static_cast<int>(m_FrameRenderData.transforms.size());
        const auto& model = tc.transform.GetModelMatrix();
        gpu.modelMatrix   = model;
        gpu.normalMatrix  = interop::glm::From(
            glm::mat4(glm::transpose(glm::inverse(glm::mat3(interop::glm::To(model)))))
        );
        m_FrameRenderData.transforms.push_back(gpu);
        return index;
    }

    int RenderContext::BuildMaterial(const UUID& materialUUID) {
        const auto it = m_MaterialIdxCache.find(materialUUID);
        if (it != m_MaterialIdxCache.end())
            return it->second;

        const auto& am = Services::GetAssetManager();
        const auto mat = am->GetMaterialInstance(materialUUID);

        const int index = m_FrameRenderData.materials.size();

        MaterialSSBO gpuData{};
        mat->ConvertToGPUFormat(gpuData);
        m_FrameRenderData.materials.push_back(gpuData);

        m_MaterialIdxCache[materialUUID] = index;

        return index;
    }

    void RenderContext::PushDrawCommand(const MeshAsset* mesh, int transformIndex, int materialIndex,uint baseInstance)
    {
        if (mesh) {
            DrawElementsIndirectCommand cmd{};
            cmd.count         = mesh->indexCount;
            cmd.instanceCount = 1;
            cmd.firstIndex    = mesh->indexOffset;
            cmd.baseVertex    = 0;
            cmd.baseInstance  = baseInstance;

            m_FrameRenderData.drawCommands.push_back(cmd);
        }

        EntityMetadata em{};
        em.transformIndex = transformIndex;
        em.materialIndex  = materialIndex;
        if (mesh) {
            em.indexCount  = static_cast<int>(mesh->indexCount);
            em.indexOffset = static_cast<int>(mesh->indexOffset);
        }

        m_FrameRenderData.entityData.push_back(em);
    }

    Vector<RenderableData> RenderContext::CollectRenderables(const Entity* entity) {
        Vector<RenderableData> result;

        if (entity->HasComponent<MeshRendererComponent>()) {
            const auto& mrc = entity->GetComponentUnchecked<MeshRendererComponent>();

            // Using same count for meshes and materials since each mesh has one material
            if (mrc.m_MeshUUIDs.size() != mrc.m_MaterialInstanceUUIDs.size()) {
                Warn("[RenderContext::CollectMeshes] MeshUUID count does not match MaterialInstanceUUIDs, Fix it!!");
                return result;
            }
            const size_t size = mrc.m_MeshUUIDs.size();
            for (size_t i = 0; i < size; i++) {
                RenderableData data;
                data.mesh = Services::GetMeshManager()->GetMeshData(mrc.m_MeshUUIDs[i]);
                data.materialUUID = mrc.m_MaterialInstanceUUIDs[i];
                result.push_back(data);
            }
        }

        return result;
    }

    void RenderContext::CollectLight(const Entity* entity) {
        if (entity->HasComponent<LightComponent>()) {
            auto& lc = entity->GetComponentUnchecked<LightComponent>();
            const auto& tc = entity->GetComponentUnchecked<TransformComponent>();

            LightSSBO gpuData{};
            lc.m_Light.ConvertToGPUFormat(tc.transform, gpuData);
            m_FrameRenderData.lights.push_back(gpuData);
        }
    }

    void RenderContext::CollectGlobalData() {
        m_FrameRenderData.globalData.GlobalAmbient = math::Vec4(0.1);
        m_FrameRenderData.globalData.lightCount[0] = 1; // TODO: remove the hardcoded value for multiple lighting!!!
    }

    void RenderContext::CleanPrevFrame() {
        // TODO: need dirty tracking system to avoid unnecessary uploads
        m_FrameRenderData.drawCommands.clear();
        m_FrameRenderData.entityData.clear();
        m_FrameRenderData.lights.clear();
        m_FrameRenderData.transforms.clear();
        // TODO: add material dirty tracker or you can't update the buffer with 'additional data'?
    }

}
