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
        sceneRenderContext->InitResources();
    }

    void Renderer::Render(Entity* camera) {
        // GPU data ready
        sceneRenderContext->CollectRenderables();

        const auto& meshManager  = Services::GetMeshManager();
        const auto& assetManager = Services::GetAssetManager();

        const auto shader = assetManager->GetShader("main");
        const auto& entities = m_Scene->GetEntities();

        shader.Bind();
        meshManager->BindUniversalVAO();
        shader.SetVec3("g_GlobalAmbient", m_Scene->GetGlobalAmbient());
        shader.SetMat4("Projection", camera->GetComponent<CameraComponent>()->m_Camera.GetProjection());
        shader.SetMat4("View", camera->GetComponent<CameraComponent>()->m_Camera.GetView());
        shader.SetVec3("viewPos", camera->GetComponent<TransformComponent>()->m_Transform.GetPosition());
        for (const auto& entity : entities | std::views::values) {
            auto& transform = entity.GetComponent<TransformComponent>()->m_Transform;
            transform.Update();
            shader.SetMat3("u_NormalMatrix", transform.GetNormalMatrix());
            shader.SetMat4("ModelMat", transform.GetModelMatrix());

            if (entity.HasComponent<LightComponent>()) {
                shader.SetVec3("lightColor", entity.GetComponent<LightComponent>()->m_Light.GetDiffuse());
                shader.SetVec3("lightPos", transform.GetPosition());
            }

            if (!entity.HasComponent<MeshComponent>()) continue;
            const auto& subMesh = meshManager->GetMeshData(entity.GetComponent<MeshComponent>()->m_MeshName);

            // if (entity.GetComponent<TagComponent>()->Tag == "Cube")
            if (entity.GetComponent<TagComponent>()->Tag == "Cube" || entity.GetComponent<TagComponent>()->Tag == "Cube2") {
                shader.SetInt("tex", 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, assetManager->GetTexture("container")->GetID());
            }
            else if (entity.GetComponent<TagComponent>()->Tag == "Light") {
                shader.SetInt("tex", 1);
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, assetManager->GetTexture("container2")->GetID());
            }
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(subMesh.m_IndexCount), GL_UNSIGNED_INT,
                           (void*)(subMesh.m_IndexOffset * sizeof(uint32_t)));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        meshManager->UnbindCurrVAO();
    }
}
