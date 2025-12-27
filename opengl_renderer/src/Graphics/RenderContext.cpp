//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/RenderContext.h"

#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Editor/EditorState.h"
#include "Graphics/Buffer.h"
#include "Graphics/Material.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"
#include "Util/Util.h"
#include "Common/RealEnum.h"
#include "Graphics/Model.h"

namespace Real {

    RenderContext::RenderContext(Scene *scene) : m_Scene(scene)
    {
    }

    void RenderContext::InitResources() {
        m_Buffers.transform.Create(m_GPUDatas.transforms,
            MAX_ENTITIES * sizeof(TransformSSBO), BufferType::SSBO
        );

        m_Buffers.texture.Create(m_GPUDatas.textures,
            MAX_ENTITIES * sizeof(GLuint64), BufferType::SSBO
        );
        m_Buffers.texture.UploadToGPU(m_GPUDatas.textures,
            m_GPUDatas.textures.size() * sizeof(GLuint64), BufferType::SSBO
        );

        m_Buffers.material.Create(m_GPUDatas.materials,
            MAX_ENTITIES * sizeof(MaterialSSBO), BufferType::SSBO
        );

        m_Buffers.light.Create(m_GPUDatas.lights,
            MAX_LIGHTS * sizeof(LightSSBO), BufferType::SSBO
        );

        m_Buffers.entityData.Create(m_GPUDatas.entityData,
            MAX_ENTITIES * sizeof(EntityMetadata), BufferType::SSBO
        );

        m_Buffers.drawCommand.Create(m_GPUDatas.drawCommands,
            MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), BufferType::SSBO
        );

        m_Buffers.camera.Create(m_GPUDatas.camera, 1 * sizeof(CameraUBO), BufferType::UBO);

        m_Buffers.globalData.Create(m_GPUDatas.globalData, 1 * sizeof(GlobalUBO), BufferType::UBO);
    }

    void RenderContext::BindGPUBuffers() const {
        m_Buffers.drawCommand.Bind(GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 0);
        m_Buffers.entityData.Bind( GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 1);
        m_Buffers.transform.Bind(  GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 2);
        m_Buffers.camera.Bind(     GL_UNIFORM_BUFFER,        BufferType::UBO,  3);
        m_Buffers.material.Bind(   GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 4);
        m_Buffers.texture.Bind(    GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 5);
        m_Buffers.light.Bind(      GL_SHADER_STORAGE_BUFFER, BufferType::SSBO, 6);
        m_Buffers.globalData.Bind( GL_UNIFORM_BUFFER,        BufferType::UBO,  7);
    }

    void RenderContext::UploadToGPU() {
        // Update per EntityMetadata
        m_Buffers.entityData.UploadToGPU(m_GPUDatas.entityData,
            m_GPUDatas.entityData.size() * sizeof(EntityMetadata), BufferType::SSBO
        );

        // Update Draw commands
        m_Buffers.drawCommand.UploadToGPU(m_GPUDatas.drawCommands,
            m_GPUDatas.drawCommands.size() * sizeof(DrawElementsIndirectCommand), BufferType::SSBO
        );

        // Update Transforms
        m_Buffers.transform.UploadToGPU(m_GPUDatas.transforms,
            m_GPUDatas.transforms.size() * sizeof(TransformSSBO), BufferType::SSBO
        );

        // Update Materials
        m_Buffers.material.UploadToGPU(m_GPUDatas.materials,
            m_GPUDatas.materials.size() * sizeof(MaterialSSBO), BufferType::SSBO
        );

        // Update Lights
        m_Buffers.light.UploadToGPU(m_GPUDatas.lights,
            m_GPUDatas.lights.size() * sizeof(LightSSBO), BufferType::SSBO
        );

        // Update Camera
        m_Buffers.camera.UploadToGPU(std::vector{m_GPUDatas.camera}, 1 * sizeof(CameraUBO), BufferType::UBO);

        // Update Global Data
        m_Buffers.globalData.UploadToGPU(std::vector{m_GPUDatas.globalData}, 1 * sizeof(GlobalUBO), BufferType::UBO);
    }

    void RenderContext::CollectRenderables() {
        CleanPrevFrame();

        const auto view = m_Scene->GetAllEntitiesWith<TransformComponent, IDComponent>();
        uint baseInstance = 0;

        for (auto [entity, transform, id] : view.each()) {
            const auto e = m_Scene->GetEntityWithUUID(id.m_UUID);
            if (!e) continue;

            const int transformIndex = PushTransform(transform);
            CollectCamera(e);
            CollectLight(e);

            for (const auto& [meshData, matUUID] : CollectRenderables(e)) {
                const int materialIndex = matUUID != 0 ? PushMaterial(matUUID) : 0;
                PushDrawCommand(meshData, transformIndex, materialIndex, baseInstance);
                ++baseInstance;
            }
        }

        // Collect others
        CollectGlobalData();

        UploadToGPU();
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
            m_GPUDatas.camera = cc.m_Camera.ConvertToGPUFormat(tc.m_Transform);
        }
    }

    int RenderContext::PushTransform(TransformComponent& tc) {
        const TransformSSBO gpuTransform = tc.m_Transform.ConvertToGPUFormat();
        const int index = static_cast<int>(m_GPUDatas.transforms.size());
        m_GPUDatas.transforms.push_back(gpuTransform);
        return index;
    }

    int RenderContext::PushMaterial(const UUID& materialUUID) {
        const auto it = m_MaterialIdxCache.find(materialUUID);
        if (it != m_MaterialIdxCache.end())
            return it->second;

        const auto& am = Services::GetAssetManager();
        const auto mat = am->GetMaterialInstance(materialUUID);

        const int index = m_GPUDatas.materials.size();
        m_GPUDatas.materials.push_back(mat->ConvertToGPUFormat());
        m_MaterialIdxCache[materialUUID] = index;

        return index;
    }

    void RenderContext::PushDrawCommand(const MeshAsset* mesh, int transformIndex, int materialIndex,uint baseInstance)
    {
        DrawElementsIndirectCommand cmd{};
        cmd.count         = mesh->m_IndexCount;
        cmd.instanceCount = 1;
        cmd.firstIndex    = mesh->m_IndexOffset;
        cmd.baseVertex    = 0;
        cmd.baseInstance  = baseInstance;

        m_GPUDatas.drawCommands.push_back(cmd);

        EntityMetadata em{};
        em.transformIndex = transformIndex;
        em.materialIndex  = materialIndex;
        em.indexCount     = static_cast<int>(mesh->m_IndexCount);
        em.indexOffset    = static_cast<int>(mesh->m_IndexOffset);

        m_GPUDatas.entityData.push_back(em);
    }

    std::vector<RenderableData> RenderContext::CollectRenderables(const Entity* entity) {
        std::vector<RenderableData> result;

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
                data.m_Mesh = Services::GetMeshManager()->GetMeshData(mrc.m_MeshUUIDs[i]);
                data.m_MaterialUUID = mrc.m_MaterialInstanceUUIDs[i];
                result.push_back(data);
            }
        }

        return result;
    }

    void RenderContext::CollectLight(const Entity* entity) {
        if (entity->HasComponent<LightComponent>()) {
            auto& lc = entity->GetComponentUnchecked<LightComponent>();
            auto& tc = entity->GetComponentUnchecked<TransformComponent>();
            m_GPUDatas.lights.push_back(lc.m_Light.ConvertToGPUFormat(tc.m_Transform));
        }
    }

    void RenderContext::CollectGlobalData() {
        m_GPUDatas.globalData.GlobalAmbient = glm::vec4(0.1);
        m_GPUDatas.globalData.lightCount[0] = 1; // TODO: remove the hardcoded value for multiple lighting!!!
    }

    void RenderContext::CleanPrevFrame() {
        // TODO: need dirty tracking system to avoid unnecessary uploads
        m_GPUDatas.drawCommands.clear();
        m_GPUDatas.entityData.clear();
        m_GPUDatas.lights.clear();
        m_GPUDatas.transforms.clear();
        // TODO: add material dirty tracker or you can't update the buffer with 'additional data'?
    }
}
