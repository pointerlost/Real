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

        m_GPUDatas.textures = Services::GetAssetManager()->UploadTexturesToGPU();
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
        const auto& am = Services::GetAssetManager();
        // Clean the data from previous frame
        CleanPrevFrame();

        const auto view = m_Scene->GetAllEntitiesWith<TransformComponent, UUID>();

        // Collect GPU-side data for EntityMetaData, Transform, MeshData, Material
        size_t i = 0;
        for (auto [entity, transform, uuid] : view.each()) {
            const auto& e = m_Scene->GetEntityWithUUID(uuid);
            if (!e) Warn("Holy shit entity doesn't have a wrapper(Entity instance)");

            // Transform GPU data
            TransformSSBO gpuTransform = transform.m_Transform.ConvertToGPUFormat();
            const int ti = static_cast<int>(m_GPUDatas.transforms.size());
            m_GPUDatas.transforms.push_back(gpuTransform);

            CollectCamera(e);
            CollectLight(e);

            int materialIndex = 0;

            const auto& meshData = CollectMeshes(e);
            for (const auto& subMesh : meshData) {
                DrawElementsIndirectCommand cmd;
                cmd.count         = subMesh.m_IndexCount;
                cmd.instanceCount = 1;
                cmd.baseVertex    = 0; // Use 0 because we've already baked the offset (idx + vertexOffset)
                cmd.firstIndex    = subMesh.m_IndexOffset;
                cmd.baseInstance  = static_cast<uint>(i++);

                m_GPUDatas.drawCommands.push_back(cmd);
                materialIndex = m_GPUDatas.materials.size();
                m_GPUDatas.materials.push_back(am->GetOrCreateMaterialInstance(subMesh.m_MaterialUUID)->ConvertToGPUFormat());

                // Entity GPU data
                EntityMetadata em;
                em.transformIndex = ti;
                em.materialIndex  = materialIndex;
                em.indexCount     = static_cast<int>(subMesh.m_IndexCount);
                em.indexOffset    = static_cast<int>(subMesh.m_IndexOffset);
                m_GPUDatas.entityData.push_back(em);
            }
        }

        // Collect others
        CollectGlobalData();

        UploadToGPU();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void RenderContext::CollectCamera(const Entity* entity) {
        /*
         * TODO: An update is required to add multiple cameras during run-time
         * (currently, adding multiple cameras may cause to crash)!!
         */
        // const auto cam = Services::GetEditorState()->camera; // TODO: if it doesn't work REMEMBER THIS SHIT!
        if (entity->HasComponent<CameraComponent>()) {
            const auto& cc = entity->GetComponent<CameraComponent>();
            const auto& tc = entity->GetComponent<TransformComponent>();
            m_GPUDatas.camera = cc->m_Camera.ConvertToGPUFormat(tc->m_Transform);
        }
    }

    std::vector<Graphics::MeshInfo> RenderContext::CollectMeshes(const Entity *entity) {
        std::vector<Graphics::MeshInfo> m_Result;

        if (entity->HasComponent<MeshRendererComponent>()) {
            const auto& mc = entity->GetComponent<MeshRendererComponent>();
            return { Services::GetMeshManager()->GetMeshData(mc->m_MeshID) };
        }

        if (entity->HasComponent<ModelComponent>()) {
            const auto& meshes = entity->GetComponent<ModelComponent>()->m_Model->m_MeshUUIDs;
            for (const auto& uuid : meshes) {
                m_Result.push_back(Services::GetMeshManager()->GetMeshData(uuid));
            }
        }

        return m_Result;
    }

    void RenderContext::CollectLight(const Entity* entity) {
        if (entity->HasComponent<LightComponent>()) {
            const auto& lc = entity->GetComponent<LightComponent>();
            const auto& tc = entity->GetComponent<TransformComponent>();
            m_GPUDatas.lights.push_back(lc->m_Light.ConvertToGPUFormat(tc->m_Transform));
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
        m_GPUDatas.materials.clear();
        m_GPUDatas.transforms.clear();
    }
}
