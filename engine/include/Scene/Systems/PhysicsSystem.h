//
// Created by pointerlost on 1/13/26.
//
#pragma once
#include "Core/ISystem.h"
#include "Scene/ISceneListener.h"
#include "Core/Utils.h"

namespace Real {
    namespace event {
        class SceneEvents;
    }

    class Scene;
    class Entity;

    namespace physics {
        class IPhysicsBackend;
    }
}

namespace Real::ecs {

    class PhysicsSystem final : public ISystem, public ISceneListener {
    public:
        explicit PhysicsSystem(Scope<physics::IPhysicsBackend> backend);
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
        Scope<physics::IPhysicsBackend> m_Backend;
        entt::registry* m_Registry = nullptr;
    };

}
