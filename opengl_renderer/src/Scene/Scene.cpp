//
// Created by pointerlost on 10/7/25.
//
#include "Scene/Scene.h"

#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Graphics/Renderer.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Real {

    Scene::Scene()
    {
    }

    void Scene::Update(opengl::Renderer* renderer) {
        // Upload GPU data
        renderer->GetRenderContext()->CollectRenderables();
    }

    Entity& Scene::CreateEntity(const std::string &tag) {
        const Entity entity{ this, m_Registry.create() };
        auto uuid = UUID();
        m_Registry.emplace<TagComponent>(entity, tag);
        m_Registry.emplace<IDComponent>(entity, uuid);
        m_Registry.emplace<TransformComponent>(entity);

        m_Entities[uuid] = entity;
        return m_Entities[uuid];
    }

    void Scene::DestroyEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

}
