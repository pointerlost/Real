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
        // GPU data ready
        const auto& data = sceneRenderContext->CollectRenderables();

        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();

        const auto shader = assetManager->GetShader("main");
        const auto& editorCam = camera->GetComponent<CameraComponent>()->m_Camera;

        sceneRenderContext->BindGPUBuffers();

        shader.Bind();
        meshManager->BindUniversalVAO();
        shader.SetVec3("g_GlobalAmbient", m_Scene->GetGlobalAmbient());
        shader.SetMat4("View", editorCam.GetView());
        shader.SetMat4("Projection", editorCam.GetProjection());
        shader.SetVec3("lightColor", glm::vec3(1.0));
        shader.SetVec3("lightPos", glm::vec3(0.0, 5.0, 5.0));
        shader.SetVec3("viewPos", camera->GetComponent<TransformComponent>()->m_Transform.GetPosition());

        if (!data.drawCommands.empty()) {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, GetRenderContext()->GetBuffers().drawCommand.GetHandle());
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(data.drawCommands.size()), 0);
        }

        meshManager->UnbindCurrVAO();
    }

    void Renderer::BindGPUBuffers() const {
        sceneRenderContext->BindGPUBuffers();
    }
}
