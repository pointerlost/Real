//
// Created by pointerlost on 10/7/25.
//
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Real {

    Scene::Scene()
    {
    }

    void Scene::Update(Entity* camera) {
        camera->GetComponent<CameraComponent>()->m_Camera.Update(camera->GetComponent<TransformComponent>()->m_Transform);
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
