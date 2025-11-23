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

namespace Real {

    RenderContext::RenderContext(Scene *scene) : m_Scene(scene)
    {
    }

    void RenderContext::InitResources() {
        m_Buffers.transform.Create(m_GPUDatas.transforms,
            MAX_ENTITIES * sizeof(TransformSSBO), opengl::BufferType::SSBO
        );

        m_GPUDatas.textures = Services::GetAssetManager()->UploadTexturesToGPU();
        m_Buffers.texture.Create(m_GPUDatas.textures,
            MAX_ENTITIES * sizeof(GLuint64), opengl::BufferType::SSBO
        );

        m_Buffers.material.Create(m_GPUDatas.materials,
            MAX_ENTITIES * sizeof(MaterialSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.light.Create(m_GPUDatas.lights,
            MAX_LIGHTS * sizeof(LightSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.entityData.Create(m_GPUDatas.entityData,
            MAX_ENTITIES * sizeof(EntityMetadata), opengl::BufferType::SSBO
        );

        m_Buffers.drawCommand.Create(m_GPUDatas.drawCommands,
            MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), opengl::BufferType::SSBO
        );

        m_Buffers.camera.Create(m_GPUDatas.camera, 1 * sizeof(CameraUBO), opengl::BufferType::UBO);

        m_Buffers.globalData.Create(m_GPUDatas.globalData, 1 * sizeof(GlobalUBO), opengl::BufferType::UBO);
    }

    void RenderContext::BindGPUBuffers() const {
        m_Buffers.drawCommand.Bind(GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 0);
        m_Buffers.entityData.Bind( GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 1);
        m_Buffers.transform.Bind(  GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 2);
        m_Buffers.camera.Bind(     GL_UNIFORM_BUFFER,        opengl::BufferType::UBO,  3);
        m_Buffers.material.Bind(   GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 4);
        m_Buffers.texture.Bind(    GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 5);
        m_Buffers.light.Bind(      GL_SHADER_STORAGE_BUFFER, opengl::BufferType::SSBO, 6);
        m_Buffers.globalData.Bind( GL_UNIFORM_BUFFER,        opengl::BufferType::UBO,  7);
    }

    void RenderContext::UploadToGPU() {
        // Update per EntityMetadata
        m_Buffers.entityData.UploadToGPU(m_GPUDatas.entityData,
            m_GPUDatas.entityData.size() * sizeof(EntityMetadata), opengl::BufferType::SSBO
        );

        // Update Draw commands
        m_Buffers.drawCommand.UploadToGPU(m_GPUDatas.drawCommands,
            m_GPUDatas.drawCommands.size() * sizeof(DrawElementsIndirectCommand), opengl::BufferType::SSBO
        );

        // Update Transforms
        m_Buffers.transform.UploadToGPU(m_GPUDatas.transforms,
            m_GPUDatas.transforms.size() * sizeof(TransformSSBO), opengl::BufferType::SSBO
        );

        // Update Materials
        m_Buffers.material.UploadToGPU(m_GPUDatas.materials,
            m_GPUDatas.materials.size() * sizeof(MaterialSSBO), opengl::BufferType::SSBO
        );

        // Is it necessary? we don't uploading textures per-frame
        m_Buffers.texture.UploadToGPU(m_GPUDatas.textures,
            m_GPUDatas.textures.size() * sizeof(GLuint64), opengl::BufferType::SSBO
        );

        // Update Lights
        m_Buffers.light.UploadToGPU(m_GPUDatas.lights,
            m_GPUDatas.lights.size() * sizeof(LightSSBO), opengl::BufferType::SSBO
        );

        // Update Camera
        m_Buffers.camera.UploadToGPU(std::vector{m_GPUDatas.camera}, 1 * sizeof(CameraUBO), opengl::BufferType::UBO);

        // Update Global Data
        m_Buffers.globalData.UploadToGPU(std::vector{m_GPUDatas.globalData}, 1 * sizeof(GlobalUBO), opengl::BufferType::UBO);
    }

    void RenderContext::CollectRenderables() {
        // Clean the data from previous frame
        CleanPrevFrame();

        const auto view = m_Scene->GetAllEntitiesWith<TransformComponent, MeshComponent, MaterialComponent>();

        // Collect GPU-side data for EntityMetaData, Transform, MeshData, Material
        size_t i = 0;
        for (auto [entity, transform, mesh, material] : view.each()) {
            EntityMetadata entityData;

            // Transform GPU data
            TransformSSBO gpuTransform = transform.m_Transform.ConvertToGPUFormat();
            const int transformIdx = static_cast<int>(m_GPUDatas.transforms.size());
            m_GPUDatas.transforms.push_back(gpuTransform);

            // Material GPU data
            MaterialSSBO gpuMaterial = material.m_Instance->ConvertToGPUFormat();
            const int materialIdx = static_cast<int>(m_GPUDatas.materials.size());
            m_GPUDatas.materials.push_back(gpuMaterial);

            const auto& meshData = Services::GetMeshManager()->GetMeshData(mesh.m_MeshName);

            DrawElementsIndirectCommand command;
            command.count = meshData.m_IndexCount;
            command.instanceCount = 1;
            command.baseVertex = 0; // Use 0 because we've already baked the offset (idx + vertexOffset)
            command.firstIndex = meshData.m_IndexOffset;
            command.baseInstance = static_cast<uint>(i++);
            m_GPUDatas.drawCommands.push_back(command);

            // Entity GPU data
            entityData.transformIndex = transformIdx;
            entityData.materialIndex  = materialIdx;
            entityData.indexCount  = static_cast<int>(meshData.m_IndexCount);
            entityData.indexOffset = static_cast<int>(meshData.m_IndexOffset);
            m_GPUDatas.entityData.push_back(entityData);
        }

        // Collect others
        CollectCamera();
        CollectLights();
        CollectGlobalData();

        UploadToGPU();
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void RenderContext::CollectCamera() {
        /*
         * TODO: An update is required to add multiple cameras during run-time
         * (currently, adding multiple cameras may cause to crash)!!
         */
        const auto camera = Services::GetEditorState()->camera;
        const auto cc = camera->GetComponent<CameraComponent>();
        const auto tc = camera->GetComponent<TransformComponent>();
        m_GPUDatas.camera = cc->m_Camera.ConvertToGPUFormat(tc->m_Transform);
    }

    void RenderContext::CollectLights() {
        const auto& view = m_Scene->GetAllEntitiesWith<LightComponent, TransformComponent>();
        for (const auto& [entity, light, transform] : view.each()) {
            m_GPUDatas.lights.push_back(light.m_Light.ConvertToGPUFormat(transform.m_Transform));
        }
    }

    void RenderContext::CollectGlobalData() {
        m_GPUDatas.globalData.GlobalAmbient = glm::vec4(0.1);
        m_GPUDatas.globalData.lightCount[0] = 1; // TODO: remove the hardcoded value
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
