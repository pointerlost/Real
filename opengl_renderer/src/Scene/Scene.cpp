//
// Created by pointerlost on 10/7/25.
//
#include "Scene/Scene.h"

#include "Core/AssetManager.h"
#include "Core/Services.h"
#include "Graphics/Material.h"
#include "Graphics/MeshManager.h"
#include "Graphics/Model.h"
#include "Graphics/Renderer.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace Real {

    Scene::Scene()
    {
    }

    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T &component) {
        static_assert(sizeof(T) == 0);
    }

    template<>
    void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<MeshRendererComponent>(Entity entity, MeshRendererComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<ModelComponent>(Entity entity, ModelComponent& component) {
        OnModelAssigned(entity, component.m_Model);
    }

    template<>
    void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<MaterialOverrideComponent>(Entity entity, MaterialOverrideComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<VelocityComponent>(Entity entity, VelocityComponent& component) {
    }

    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component) {
    }

    void Scene::Update(opengl::Renderer* renderer) {
        // Upload GPU data
        renderer->GetRenderContext()->CollectRenderables();
    }

    Entity& Scene::CreateEntity(const std::string &tag) {
        const auto& am = Services::GetAssetManager();
        const auto& mm = Services::GetMeshManager();
        const Entity entity{ this, m_Registry.create() };
        auto uuid = UUID();
        m_Registry.emplace<TagComponent>(entity, tag);
        m_Registry.emplace<IDComponent>(entity, uuid);
        m_Registry.emplace<TransformComponent>(entity);
        m_Registry.emplace<MeshRendererComponent>(entity, mm->GetPrimitiveUUID("cube"), am->GetOrCreateMaterialBase("Marble009")->m_ID);
        // TODO: add fallback for mesh and material

        m_Entities[uuid] = entity;
        return m_Entities[uuid];
    }

    void Scene::DestroyEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

    Entity& Scene::CreateLight(const std::string &entityTag, LightType type) {
        auto& entity = CreateEntity(entityTag);
        entity.AddComponent<LightComponent>().m_Light = Light{type};
        return entity;
    }

    Entity* Scene::GetEntityWithUUID(UUID uuid) {
        if (!m_Entities.contains(uuid)) {
            Warn("Entity doesn't exists!");
        }
        return &m_Entities[uuid];
    }

    void Scene::OnModelAssigned(Entity parent, Ref<Model> model) {
    }
}
