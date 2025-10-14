//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/RenderContext.h"
#include "Core/Services.h"
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
        m_Buffers.transform.UploadToGPU(m_GPUDatas.transforms,
            m_GPUDatas.transforms.size() * sizeof(TransformSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.material.Create(m_GPUDatas.materials,
            MAX_ENTITIES * sizeof(MaterialSSBO), opengl::BufferType::SSBO
        );
        m_Buffers.material.UploadToGPU(m_GPUDatas.materials,
            m_GPUDatas.materials.size() * sizeof(MaterialSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.light.Create(m_GPUDatas.lights, MAX_LIGHTS * sizeof(LightSSBO), opengl::BufferType::SSBO
        );
        m_Buffers.light.UploadToGPU(m_GPUDatas.lights,
            m_GPUDatas.lights.size() * sizeof(LightSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.entityData.Create(m_GPUDatas.entityData,
            MAX_ENTITIES * sizeof(EntityMetadata), opengl::BufferType::SSBO
        );
        m_Buffers.entityData.UploadToGPU(m_GPUDatas.entityData,
            m_GPUDatas.entityData.size() * sizeof(EntityMetadata), opengl::BufferType::SSBO
        );

        m_Buffers.drawCommand.Create(m_GPUDatas.drawCommands,
            MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), opengl::BufferType::SSBO
        );
        m_Buffers.drawCommand.UploadToGPU(m_GPUDatas.drawCommands,
            m_GPUDatas.drawCommands.size() * sizeof(DrawElementsIndirectCommand), opengl::BufferType::SSBO
        );

        // m_Buffers.camera.Create(std::vector{m_GPUDatas.camera}, 1 * sizeof(CameraUBO), opengl::BufferType::UBO);
        // m_Buffers.camera.UploadToGPU(std::vector{m_GPUDatas.camera}, 1 * sizeof(CameraUBO), opengl::BufferType::UBO);
    }

    void RenderContext::BindGPUBuffers() const {
        m_Buffers.drawCommand.Bind(opengl::BufferType::SSBO, 0);
        m_Buffers.entityData.Bind(opengl::BufferType::SSBO, 1);
        m_Buffers.transform.Bind(opengl::BufferType::SSBO, 2);
        // m_Buffers.camera.Bind(opengl::BufferType::UBO, 3);
        m_Buffers.material.Bind(opengl::BufferType::SSBO, 4);
        m_Buffers.light.Bind(opengl::BufferType::SSBO, 5);
    }

    void RenderContext::UploadToGPU() {
        m_Buffers.transform.UploadToGPU(m_GPUDatas.transforms,
            m_GPUDatas.transforms.size() * sizeof(TransformSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.material.UploadToGPU(m_GPUDatas.materials,
            m_GPUDatas.materials.size() * sizeof(MaterialSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.light.UploadToGPU(m_GPUDatas.lights,
            m_GPUDatas.lights.size() * sizeof(LightSSBO), opengl::BufferType::SSBO
        );

        m_Buffers.entityData.UploadToGPU(m_GPUDatas.entityData,
            m_GPUDatas.entityData.size() * sizeof(EntityMetadata), opengl::BufferType::SSBO
        );

        m_Buffers.drawCommand.UploadToGPU(m_GPUDatas.drawCommands,
            m_GPUDatas.drawCommands.size() * sizeof(DrawElementsIndirectCommand), opengl::BufferType::SSBO
        );
    }

    const GPUData& RenderContext::CollectRenderables() {
        // Clean the data from previous frame
        CleanPrevFrame();

        const auto view = m_Scene->GetAllEntitiesWith<TransformComponent, MeshComponent, MaterialComponent>();

        // Collect GPU-side data for EntityMetaData, Transform, MeshData, Material
        size_t i = 0;
        for (auto [entity, transform, mesh, material] : view.each()) {
            EntityMetadata entityData;

            // Transform GPU data
            TransformSSBO gpuTransform = transform.m_Transform.ConvertToGPUFormat();
            const int transformIdx = (int)m_GPUDatas.transforms.size();
            m_GPUDatas.transforms.push_back(gpuTransform);

            // Material GPU data
            MaterialSSBO gpuMaterial = material.m_Instance->ConvertToGPUFormat();
            const int materialIdx = (int)m_GPUDatas.materials.size();
            m_GPUDatas.materials.push_back(gpuMaterial);

            const auto& meshData = Services::GetMeshManager()->GetMeshData(mesh.m_MeshName);

            DrawElementsIndirectCommand command;
            command.count = meshData.m_IndexCount;
            command.instanceCount = 1;
            command.baseVertex = meshData.m_IndexOffset;
            command.firstIndex = 0;
            command.baseInstance = (uint)i++;
            m_GPUDatas.drawCommands.push_back(command);

            // Entity GPU data
            entityData.transformIndex = transformIdx;
            entityData.materialIndex  = materialIdx;
            entityData.indexCount  = (int)meshData.m_IndexCount;
            entityData.indexOffset = (int)meshData.m_IndexOffset;
            m_GPUDatas.entityData.push_back(entityData);
        }

        UploadToGPU();

        // Collect others

        // CollectCamera();
        // CollectLights();

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        return m_GPUDatas;
    }

    void RenderContext::CollectCamera() {
        /* TODO: An update is required to add multiple cameras during run-time
         * (currently, adding multiple cameras may cause to crash)!!
         */
        for (const auto& view = m_Scene->GetAllEntitiesWith<CameraComponent>(); auto [entity, camera] : view.each()) {
            const CameraUBO gpuData = camera.m_Camera.ConvertToGPUFormat();
            m_GPUDatas.camera = gpuData;
        }
    }

    void RenderContext::CollectLights() {
        for (const auto& view = m_Scene->GetAllEntitiesWith<LightComponent>(); auto [entity, light] : view.each()) {
            LightSSBO gpuData = light.m_Light.ConvertToGPUFormat();
            m_GPUDatas.lights.push_back(gpuData);
        }
    }

    void RenderContext::CleanPrevFrame() {
        m_GPUDatas.drawCommands.clear();
        m_GPUDatas.entityData.clear();
        m_GPUDatas.lights.clear();
        m_GPUDatas.materials.clear();
        m_GPUDatas.transforms.clear();
    }
}
