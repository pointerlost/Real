//
// Created by pointerlost on 2/16/26.
//
#include <Platform/opengl/OpenGLRenderer.h>

#include "Core/AssetManager.h"
#include "Core/Services.h"

namespace Real::platform::opengl {

    void OpenGLRenderer::Init() {
        m_RenderContext(CreateScope<RenderContext>());
    }

    void OpenGLRenderer::Render(Scene* scene, Entity* camera) {
        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();
        const auto shader = assetManager->GetShader("main");

        // Bind gpu buffer to binding points
        BindGPUBuffers();

        // Bind Shader and VAO
        shader.Bind();
        meshManager->BindUniversalVAO();

        // Set uniforms

        // Draw indirect
        const auto& gpuData = m_RenderContext->GetGPURenderData();
        if (!gpuData.drawCommands.empty()) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, GetRenderContext()->GetBuffers().drawCommand.GetHandle());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(gpuData.drawCommands.size()), 0);
        }

        meshManager->UnbindCurrVAO();
    }

    void OpenGLRenderer::Shutdown() {
    }

    void OpenGLRenderer::BindGPUBuffers() {
        m_RenderContext->BindGPUBuffers();
    }
}
