//
// Created by pointerlost on 2/16/26.
//
#include <Platform/OpenGL/OpenGLRenderer.h>
#include "../../../include/Assets/AssetManager.h"
#include "Core/Logger.h"
#include "Core/Services.h"
#include "Graphics/MeshManager.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Shader.h"

namespace {
    constexpr int MAX_ENTITIES = 16384;
    constexpr int MAX_LIGHTS = 512;
}

namespace Real::platform::opengl {

    OpenGLRenderer::OpenGLRenderer(Scope<core::IRenderDevice> device)
        : m_Device(std::move(device))
    {
    }

    void OpenGLRenderer::Init() {
        m_RenderContext = CreateScope<RenderContext>();
        Info("OpenGLRenderer::Init successfully!");
    }

    void OpenGLRenderer::Render(Scene* scene, Entity* camera) {
        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();
        const auto shader = assetManager->GetShader("main");

        m_RenderContext->CollectRenderables(scene);

        UploadToGPU();

        // Bind gpu buffer to binding points
        BindGPUBuffers();

        // Bind Shader and VAO
        shader.Bind();
        meshManager->BindUniversalVAO();

        // Set uniforms

        // Draw indirect
        const auto& gpuData = m_RenderContext->GetGPURenderData();
        if (!gpuData.drawCommands.empty()) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_Buffers.drawCommand.GetHandle());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(gpuData.drawCommands.size()), 0);
        }

        meshManager->UnbindCurrVAO();
    }

    void OpenGLRenderer::Shutdown() {
    }

    void OpenGLRenderer::BeginFrame() {
        m_Device->ClearColor({0.07f, 0.07f, 0.07f, 1.0f});
        m_Device->Clear(graphics::ClearFlags::Color);
    }

    void OpenGLRenderer::EndFrame() {
    }

    void OpenGLRenderer::InitResources() {
        const auto& gpuData = m_RenderContext->GetGPURenderData();
        m_Buffers.transform.Create(gpuData.transforms.data(),
            MAX_ENTITIES * sizeof(TransformSSBO), BufferType::SSBO
        );

        m_Buffers.texture.Create(gpuData.textures.data(),
            MAX_ENTITIES * sizeof(graphics::BindlessHandle), BufferType::SSBO
        );
        m_Buffers.texture.Upload(gpuData.textures.data(),
            gpuData.textures.size() * sizeof(graphics::BindlessHandle)
        );

        m_Buffers.material.Create(gpuData.materials.data(),
            MAX_ENTITIES * sizeof(MaterialSSBO), BufferType::SSBO
        );

        m_Buffers.light.Create(gpuData.lights.data(),
            MAX_LIGHTS * sizeof(LightSSBO), BufferType::SSBO
        );

        m_Buffers.entityData.Create(gpuData.entityData.data(),
            MAX_ENTITIES * sizeof(EntityMetadata), BufferType::SSBO
        );

        m_Buffers.drawCommand.Create(gpuData.drawCommands.data(),
            MAX_ENTITIES * sizeof(DrawElementsIndirectCommand), BufferType::SSBO
        );

        m_Buffers.camera.Create(&gpuData.camera, 1 * sizeof(FrameUBO), BufferType::UBO);

        m_Buffers.globalData.Create(&gpuData.globalData, 1 * sizeof(GlobalUBO), BufferType::UBO);
    }

    void OpenGLRenderer::BindGPUBuffers() const {
        m_Buffers.drawCommand.Bind(GL_SHADER_STORAGE_BUFFER, 0);
        m_Buffers.entityData.Bind( GL_SHADER_STORAGE_BUFFER, 1);
        m_Buffers.transform.Bind(  GL_SHADER_STORAGE_BUFFER, 2);

        m_Buffers.camera.Bind(     GL_UNIFORM_BUFFER,        3);

        m_Buffers.material.Bind(   GL_SHADER_STORAGE_BUFFER, 4);
        m_Buffers.texture.Bind(    GL_SHADER_STORAGE_BUFFER, 5);
        m_Buffers.light.Bind(      GL_SHADER_STORAGE_BUFFER, 6);

        m_Buffers.globalData.Bind( GL_UNIFORM_BUFFER,        7);
    }

    void OpenGLRenderer::UploadToGPU() {
        const auto& gpuData = m_RenderContext->GetGPURenderData();
        // Update per EntityMetadata
        m_Buffers.entityData.Upload(gpuData.entityData.data(),
            gpuData.entityData.size() * sizeof(EntityMetadata)
        );

        // Update Draw commands
        m_Buffers.drawCommand.Upload(gpuData.drawCommands.data(),
            gpuData.drawCommands.size() * sizeof(DrawElementsIndirectCommand)
        );

        // Update Transforms
        m_Buffers.transform.Upload(gpuData.transforms.data(),
            gpuData.transforms.size() * sizeof(TransformSSBO)
        );

        // Update Materials
        m_Buffers.material.Upload(gpuData.materials.data(),
            gpuData.materials.size() * sizeof(MaterialSSBO)
        );

        // Update Lights
        m_Buffers.light.Upload(gpuData.lights.data(),
            gpuData.lights.size() * sizeof(LightSSBO)
        );

        // Update Camera
        m_Buffers.camera.Upload(&gpuData.camera, 1 * sizeof(FrameUBO));

        // Update Global Data
        m_Buffers.globalData.Upload(&gpuData.globalData, 1 * sizeof(GlobalUBO));
    }
}
