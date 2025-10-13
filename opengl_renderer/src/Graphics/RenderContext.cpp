//
// Created by pointerlost on 10/13/25.
//
#include "Graphics/RenderContext.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/MeshManager.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace Real {

    RenderContext::RenderContext(Scene *scene) : m_Scene(scene)
    {
    }

    void RenderContext::InitResources() {
    }

    void RenderContext::CollectRenderables() {
        const auto view = m_Scene->GetAllEntitiesWith<TransformComponent, MeshComponent, MaterialComponent>();

        // Collect GPU-side data for EntityMetaData, Transform, MeshData, Material
        size_t i = 0;
        for (const auto& [entity, transform, mesh, material] : view.each()) {
            EntityMetadata entityData;

            // Transform GPU data
            TransformSSBO gpuTransform = transform.m_Transform.ConvertToGPUFormat();
            const uint transformIdx = static_cast<uint>(m_GPUBuffers.transforms.size());
            m_GPUBuffers.transforms.push_back(gpuTransform);

            // Material GPU data
            MaterialSSBO gpuMaterial = material.m_Instance->ConvertToGPUFormat();
            const uint materialIdx = static_cast<uint>(m_GPUBuffers.materials.size());
            m_GPUBuffers.materials.push_back(gpuMaterial);

            const auto& meshData = Services::GetMeshManager()->GetMeshData(mesh.m_MeshName);

            DrawElementsIndirectCommand command;
            command.count = meshData.m_IndexCount;
            command.baseInstance = 1;
            command.baseVertex = meshData.m_IndexOffset;
            command.firstIndex = 0;
            command.baseInstance = i++;

            // Entity GPU data
            entityData.transformIndex = transformIdx;
            entityData.materialIndex  = materialIdx;
            entityData.indexCount  = meshData.m_IndexCount;
            entityData.indexOffset = meshData.m_IndexOffset;
            m_GPUBuffers.entityData.push_back(entityData);
        }

        // Collect others
        CollectCamera();
        CollectLights();

        // TODO: sync before GPU access with (glMemoryBarrier)
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void RenderContext::CollectCamera() {
        /* TODO: An update is required to add multiple cameras during run-time
         * (currently, adding multiple cameras may cause to crash)!!
         */
        for (const auto& view = m_Scene->GetAllEntitiesWith<CameraComponent>(); auto [entity, camera] : view.each()) {
            const CameraUBO gpuData = camera.m_Camera.ConvertToGPUFormat();
            m_GPUBuffers.camera = gpuData;
        }
    }

    void RenderContext::CollectLights() {
        for (const auto& view = m_Scene->GetAllEntitiesWith<LightComponent>(); auto [entity, light] : view.each()) {
            LightSSBO gpuData = light.m_Light.ConvertToGPUFormat();
            m_GPUBuffers.lights.push_back(gpuData);
        }
    }
}
