//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "Core/ISystem.h"
#include "Scene/ISceneListener.h"

namespace Real {
    namespace core {
        class IPhysicsBackend;
    }

    namespace event {
        class SceneEvents;
    }

    class Scene;
    class Entity;
}

namespace Real::ecs {

    class PhysicsSystem final : public ISystem, public ISceneListener {
    public:
        explicit PhysicsSystem(core::IPhysicsBackend& backend);
        void Init() override;
        void Update(entt::registry& registry, f32 deltaTime) override;
        void Shutdown() override;

        void OnSceneAttach(entt::registry& registry, event::SceneEvents& events) override;
        void OnSceneDetach(entt::registry& registry, event::SceneEvents& events) override;

    private:
        void OnColliderAdded(const entt::entity& e);
        void OnColliderRemoved(entt::entity& e);
        void OnColliderChanged(const entt::entity& e);
        void OnPhysicsBodyAdded(const entt::entity& e);
        void OnPhysicsBodyChanged(const entt::entity& e);

        void RebuildCollider(const entt::entity &e);

        void SubmitColliderDebug(const entt::entity& e);

        void RegisterEventCallbacks(event::SceneEvents& events);

        void SyncTransform(const entt::entity& entity);
        void SyncCollider(entt::entity& entity);

    private:
        core::IPhysicsBackend& m_Backend;
        entt::registry* m_Registry = nullptr;
    };

}
