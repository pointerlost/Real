//
// Created by pointerlost on 10/7/25.
//
#include "Graphics/Renderer.h"

#include <ranges>

#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Transformations.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Real::opengl {

    Renderer::Renderer(Scene* scene) : m_Scene(scene),
        sceneRenderContext(CreateScope<RenderContext>(m_Scene))
    {
    }

    void Renderer::Render(Entity* camera) {
        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();
        // GPU data ready
        const auto& renderables = sceneRenderContext->CollectRenderables();
        const auto shader = assetManager->GetShader("main");

        // Bind gpu buffer to binding points
        sceneRenderContext->BindGPUBuffers();

        // Bind Shader and VAO
        shader.Bind();
        meshManager->BindUniversalVAO();

        // Set uniforms
        shader.SetVec3("g_GlobalAmbient", m_Scene->GetGlobalAmbient());
        shader.SetInt("uLightCount", 1);
        shader.SetVec3("viewPos", camera->GetComponent<TransformComponent>()->m_Transform.GetPosition());

        if (!renderables.drawCommands.empty()) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, GetRenderContext()->GetBuffers().drawCommand.GetHandle());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(renderables.drawCommands.size()), 0);
        }

        meshManager->UnbindCurrVAO();
    }

    void Renderer::BindGPUBuffers() const {
        sceneRenderContext->BindGPUBuffers();
    }
}
