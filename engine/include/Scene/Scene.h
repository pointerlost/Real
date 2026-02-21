//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "Core/Utils.h"
#include "Core/UUID.h"
#include "Event/SceneEvents.h"
#include "Graphics/Light.h"

namespace Real::core {
    class IRenderer;
}

namespace Real {
    class Entity;
    struct Model;

    namespace opengl {
        class OpenGLRenderer;
    }
}

namespace Real {

    class Scene {
    public:
        Scene();
        void Update(const core::IRenderer* renderer);

        Entity& CreateEntity(const String& tag = String());
        void DestroyEntity(entt::entity entity);

        entt::registry& GetRegistry() { return m_Registry; }

        template <typename ...Components>
        auto GetAllEntitiesWith() {
            return m_Registry.view<Components...>();
        }

        std::unordered_map<UUID, Entity>& GetEntities() { return m_Entities; }
        size_t GetEntityCount() const { return m_Entities.size(); }

        Entity& CreateLight(const String& entityTag, Light::Mode mode = Light::Mode::POINT);

        Entity* GetEntityWithUUID(UUID uuid);
        void HandleModelAssigned(Entity& parent, const Ref<Model>& model);

        template <typename T>
        void OnComponentAdded(Entity& entity, T& component);

        template<typename T>
        void OnComponentConstructed(entt::registry& registry, entt::entity entity);

        event::SceneEvents& GetEvents() { return m_Events; }

    private:
        entt::registry m_Registry;
        std::unordered_map<UUID, Entity> m_Entities;

    private:
        event::SceneEvents m_Events;
    };
}
