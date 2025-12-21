//
// Created by pointerlost on 10/7/25.
//
#include "Graphics/Renderer.h"

#include <ranges>

#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Core/Timer.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Transformations.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real::opengl {

    Renderer::Renderer(Scene* scene) : m_Scene(scene),
        m_SceneRenderContext(CreateScope<RenderContext>(m_Scene))
    {
    }

    void Renderer::Render(Entity* camera) {
        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();
        const auto shader = assetManager->GetShader("main");

        // Bind gpu buffer to binding points
        m_SceneRenderContext->BindGPUBuffers();

        // Bind Shader and VAO
        shader.Bind();
        meshManager->BindUniversalVAO();

        // Set uniforms

        const auto& gpuData = m_SceneRenderContext->GetGPURenderData();
        if (!gpuData.drawCommands.empty()) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, GetRenderContext()->GetBuffers().drawCommand.GetHandle());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(gpuData.drawCommands.size()), 0);
        }

        meshManager->UnbindCurrVAO();
    }

    void Renderer::BindGPUBuffers() const {
        m_SceneRenderContext->BindGPUBuffers();
    }
}
