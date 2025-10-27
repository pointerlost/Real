//
// Created by pointerlost on 10/7/25.
//
#pragma once
#include "entt/entt.hpp"
#include "Core/Logger.h"
#include "Core/UUID.h"
#include <glm/ext.hpp>
#include "Graphics/Light.h"

namespace Real {
    class Entity;

    namespace opengl {
        class Renderer;
    }
}

namespace Real {

    class Scene {
    public:
        Scene();
        void Update(opengl::Renderer* renderer);

        Entity& CreateEntity(const std::string& tag = std::string());
        void DestroyEntity(entt::entity entity);

        entt::registry& GetRegistry() { return m_Registry; }

        template <typename ...Components>
        auto GetAllEntitiesWith() {
            return m_Registry.view<Components...>();
        }

        std::unordered_map<UUID, Entity>& GetEntities() { return m_Entities; }
        size_t GetEntityCount() const { return m_Entities.size(); }

        Entity& CreateLight(const std::string& entityTag, LightType type = LightType::POINT);

        Entity* GetEntityWithUUID(UUID uuid);

    private:
        entt::registry m_Registry;
        std::unordered_map<UUID, Entity> m_Entities;
    };
}
